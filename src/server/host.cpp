#include "/home/shark/code/SmallStore/include/databaseclass.h"
#include <mutex>

Database serverdb;
std::shared_mutex dbmutex;

int createSocket()
{

	int serversockfd = socket(AF_INET, SOCK_STREAM, 0);
	int flag = 1;
    	setsockopt(serversockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
	if (serversockfd == -1)
	{
		std::cout << "SOCKET CREATION FAILED, EXITED CODE -1";
		return -1;
	}
	return serversockfd;
}
sockaddr_in bindSocket(int serversockfd)
{

	sockaddr_in serverinfo;

	serverinfo.sin_family = AF_INET;
	serverinfo.sin_port = htons(8081);
	serverinfo.sin_addr.s_addr = INADDR_ANY;

	if (bind(serversockfd, (struct sockaddr *)&serverinfo, sizeof(serverinfo)) == -1)
	{
		close(serversockfd);
		throw std::runtime_error("FAILED TO BIND SOCKET");
	}
	return serverinfo;
}
bool validateTokens(std::vector<std::string>& tokens){
		if(tokens.empty()) return false;
		if(tokens[0]=="GET" && tokens.size() != 4) return false;
		if(tokens[0]=="GETBIN" && tokens.size() != 2) return false;
		if(tokens[0]=="PUT" && tokens.size() != 3 && tokens.size() != 4) return false;
		if(tokens[0]=="PUTIN" && tokens.size() != 5) return false;
		if(tokens[0]=="FILL" && tokens.size() != 4) return false;
		if(tokens[0]=="APPEND" && tokens.size() != 4) return false;
		if(tokens[0]=="REMOVE" && tokens.size() != 2) return false;
		if(tokens[0]=="CACHEON" && tokens.size() != 1) return false;
		if(tokens[0]=="SAVE" && tokens.size() != 1) return false;
		if(tokens[0]=="LOAD" && tokens.size() != 1) return false;
		if(tokens[0]=="CLOSE" && tokens.size() != 1) return false;
		if(tokens[0]=="INSERT" && tokens.size() != 4) return false;
	        if(tokens[0]=="CLEARSET" && tokens.size()!= 2) return false; 	

		return true;
}
void databaseOperations(std::vector<std::string>& tokens, int& clientsockfd){
	
	std::string result;
	std::string operation = tokens[0];

	if (operation == "GET"){

		if(tokens.size()<3) return;

		std::shared_lock lock(dbmutex);
		std::string datatype = tokens[1];
		std::string key = tokens[2]; 
		std::string field = tokens[3];

		if (datatype == "ARRAY"){
			int index = std::stoi(field);
			result = serverdb.getString(key, "", index);
		}
		if (datatype == "HASHMAP"){
			result = serverdb.getString(key, field, 0);
		}
		if (datatype == "STRING"){
			result = serverdb.getString(key, "", 0);
		}
		if (datatype == "SET"){
			result = serverdb.getString(key, field, 0);
		} 
		if (!result.empty()){
			send(clientsockfd, result.c_str(), result.length(), 0);
		}

		tokens.clear();

	}
	if (operation == "GETBIN"){
		
		if (tokens.size()<2 || tokens.size() > 2) return;
		std::shared_lock lock(dbmutex);
		std::string key = tokens[1];
		std::vector<char> binData = serverdb.getBinary(key);
		if(!binData.empty()){
			uint32_t size = htonl(binData.size());
			send(clientsockfd, &size, sizeof(size), 0);
			send(clientsockfd, binData.data(), binData.size(), 0); 
		}
		tokens.clear();
	}
	if (operation == "PUT"){
		
		if (tokens.size()>5 || tokens.size() < 3) return; 

		std::unique_lock lock(dbmutex);
		std::string datatype = tokens[1];
		std::string key = tokens[2];
		
		if (datatype == "HASHMAP"){
			serverdb.putHashmap(key);
		}
		else if (datatype == "ARRAY"){
			serverdb.putArray(key);
		}
		else if (datatype == "STRING"){
			serverdb.putString(key);
		}
		else if (datatype == "SET"){
			serverdb.putSet(key); 
		} 
		else if (datatype == "BIN"){
			std::string srcfilepath = tokens[3];
			std::string dstfilepath = tokens[4];
			serverdb.putBinary(key, srcfilepath, dstfilepath);
		}
	
		tokens.clear();
	}
	if (operation == "CLEARSET"){
		if (tokens.size() > 2 || tokens.size() < 2) return;
		std::unique_lock lock(dbmutex); 
		std::string key = tokens[1];
		serverdb.clearSet(key);

	}  
	if (operation == "INSERT"){
		if (tokens.size() > 4 || tokens.size() < 4) return; 
		std::unique_lock lock(dbmutex); 
		std::string datatype = tokens[1];
		std::string key = tokens[2];
		std::string value = tokens[3]; 
		
		if (datatype == "SET"){
			serverdb.insertSet(key,value);
		} 
	}  
	if (operation == "PUTIN"){

		if(tokens.size()<5 || tokens.size()>5) return;

		std::unique_lock lock(dbmutex);
		
		std::string datatype = tokens[1];
		std::string key = tokens[2];
		std::string field = tokens[3];
		std::string value = tokens[4];

		if (datatype == "HASHMAP"){
			serverdb.putInHashmap(key, field, value);
		}
		else if (datatype == "ARRAY"){
			int position = std::stoi(field);
			serverdb.putInArray(key, position, value);
		}

		tokens.clear();
	}
	if (operation == "FILL"){
		if (tokens.size()>4||tokens.size()<4) return;

		std::unique_lock lock(dbmutex);
		
		std::string datatype = tokens[1];
		std::string key = tokens[2];
		std::string value = tokens[3];

		if (datatype == "STRING"){
			serverdb.fillString(key, value);
		}
		tokens.clear();
	}
	if (operation == "APPEND"){
		if(tokens.size()<4||tokens.size()>4) return;

		std::unique_lock lock(dbmutex);
		
		std::string datatype = tokens[1];
		std::string key = tokens[2];
		std::string value = tokens[3];

		if (datatype == "ARRAY"){
			serverdb.appendToArray(key, value);
		}
		
		tokens.clear();
	}
	if(operation == "CACHEON"){
		if(tokens.size()>1 || tokens.size()<1) return;
		serverdb.evictFromLRUCache();
		tokens.clear();
	} 
	if (operation == "REMOVE") {
		if(tokens.size()>2 || tokens.size()<2) return;
		std::string key = tokens[1];
		serverdb.remove(key);
		tokens.clear();
	}
	if (operation == "PSAVE"){
		if(tokens.size()>1 || tokens.size()<1) return;	
		serverdb.periodicalSaveToJSON();
		tokens.clear(); 
	}  
	if (operation == "SAVE"){
		if(tokens.size()>1 || tokens.size()<1) return;
		serverdb.saveToJSON();
		tokens.clear();
	}
	if (operation == "LOAD"){
		if(tokens.size()>1 || tokens.size()<1) return;
		serverdb.loadFromJSON();
		tokens.clear();
	}
	if (operation == "CLOSE") {	
		if(tokens.size()>1 || tokens.size()<1) return;
		std::cout << "CLOSING CONNECTION..." << std::endl;
		close(clientsockfd);
	}
}

int handleClient(int clientsockfd){
	while (true){

		char buffer[2048];
		size_t bufferSize = sizeof(buffer); 
		memset(buffer, 0, bufferSize);
		ssize_t bytesrecvd = recv(clientsockfd, buffer, bufferSize - 1, 0);
		std::string stringbuffer = std::string(buffer);

		if (bytesrecvd > 0){
			if (bytesrecvd == sizeof(buffer)-1){
				std::vector<char> vec; 
				vec.assign(buffer, buffer + bufferSize);
				vec.reserve(1024000);
				char tempbuffer[4096];
				ssize_t moreBytes;
				do{
					moreBytes = recv(clientsockfd, tempbuffer, sizeof(tempbuffer), 0);
					if (moreBytes>0){
						vec.insert(vec.end(), tempbuffer, tempbuffer+moreBytes);
					}
				 } while (moreBytes == sizeof(tempbuffer)); 
				
			}
			

			stringbuffer.assign(buffer, bytesrecvd);
			
			buffer[bytesrecvd] = '\0';
			std::cout << "RECIEVED FROM CLIENT:\n"<< buffer << "\n"<< std::endl;
			std::istringstream fullstream(stringbuffer);
			std::string line;

			while (std::getline(fullstream, line)){
				if (line.empty()) continue;

				std::istringstream linestream(line);
				std::vector<std::string> tokens;
				std::string token;

				while (linestream >> token)
				{
					tokens.push_back(token);
				}
				if (tokens.empty()) continue;

				validateTokens(tokens);
				databaseOperations(tokens, clientsockfd);
			}
		}
		else if (bytesrecvd == 0){
			std::cout << "CLIENT DISCONNECTED\n" << std::endl;
			close(clientsockfd);
			break;
		}
		else{
			close(clientsockfd);
			throw std::runtime_error("ERROR WHILE RECIEVING DATA");
		}		
	}
	return 1; 	
}
int acceptConnections(int serversockfd, sockaddr_in &serverinfo){
	std::vector<std::thread> threads;

	if (listen(serversockfd, 3) == -1){
		std::cout << "COULD NOT OPEN FOR CONNECTION, EXITED CODE -1";
		close(serversockfd);
		return false;
	}
	sockaddr_in clientinfo;
	socklen_t clientaddrlen = sizeof(clientinfo);
	while (true){
		int clientsockfd = accept(serversockfd, (struct sockaddr *)&clientinfo, &clientaddrlen);
		if (clientsockfd != -1){
			int flag = 1;
			setsockopt(clientsockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
			threads.emplace_back(handleClient, clientsockfd);
			threads.back().detach();
		}
		else if (clientsockfd == 1){
			std::cout << "acceptConnections FAILED";
			close(clientsockfd);
		}
	}
	return 1;
}
void startServer(){

	std::cout << "STARTING SERVER...\n" << std::endl;
	int sockfd = createSocket();
	sockaddr_in serverinfo = bindSocket(sockfd);
	int clientsockfd = acceptConnections(sockfd, serverinfo);

	close(sockfd);
}
int main(){
	startServer();
}
