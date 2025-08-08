#include "/home/shark/code/SmallStore/include/clientclass.h"
#include <sys/types.h>

int SmallStoreClient::createSocket()
{

	int clientsockfd = socket(AF_INET, SOCK_STREAM, 0);
	int flag = 1; 
	setsockopt(clientsockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
	if (clientsockfd == -1)
	{
		throw std::runtime_error("FAILED TO CREATE SOCKET, EXITED CODE -1");
	}
	return clientsockfd;


}

sockaddr_in SmallStoreClient::serverStructure()
{

	sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8081);

	if (inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr) <= 0)
	{
		throw std::runtime_error("IP COULD NOT CONVERT TO BINARY FORMAT");
	}
	return serveraddr;
}

bool SmallStoreClient::connectionHandling(sockaddr_in serveraddr)
{

	socklen_t serveraddrlen = sizeof(sockaddr_in);
	if (connect(sockfd, (struct sockaddr *)&serveraddr, serveraddrlen) == -1)
	{
		close(sockfd);
		throw std::runtime_error("FAILED TO CONNECT TO SERVER, EXITED CODE -1");
	}
	connected = true;
	return true;
}

SmallStoreClient::SmallStoreClient() : sockfd(-1), connected(false) {}

SmallStoreClient::~SmallStoreClient()
{
	if (connected)
	{
		close(sockfd);
	}
}

void SmallStoreClient::SmallClientStart()
{
	try
	{
		sockfd = createSocket();
		sockaddr_in serveraddr = serverStructure();
		connectionHandling(serveraddr);
	}
	catch (const std::exception &e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

void SmallStoreClient::sendReq(const std::string &req)
{
	if (connected && sockfd != -1)
	{
		const char *recvdata;
		const char *reqdata = req.c_str();
		size_t reqlen = req.length();
		try
		{
			if(strlen(reqdata)<2048){
				ssize_t bytes_sent = send(sockfd, reqdata, reqlen, 0);
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
	}
}
std::string SmallStoreClient::recvIntoStr()
{

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytes_recv = recv(sockfd, buffer, sizeof(buffer), 0);

	return std::string(buffer);
}
int SmallStoreClient::recvIntoInt()
{

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytes_recv = recv(sockfd, buffer, sizeof(buffer), 0);

	return std::stoi(std::string(buffer));
}
double SmallStoreClient::recvIntoDouble()
{

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytes_recv = recv(sockfd, buffer, sizeof(buffer), 0);

	return std::stod(std::string(buffer));
}
float SmallStoreClient::recvIntoFloat()
{

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytes_recv = recv(sockfd, buffer, sizeof(buffer), 0);

	return std::stof(std::string(buffer));
}
void SmallStoreClient::recvIntoBin(std::string filename){

    uint32_t size;
    ssize_t bytes_recv = recv(sockfd, &size, sizeof(size), MSG_WAITALL);
    if (bytes_recv != sizeof(size)) {
        throw std::runtime_error("Failed to receive size");
    }
    size = ntohl(size); 
    
    std::vector<char> vec(size);
    size_t total_received = 0;
    
    while (total_received < size) {
        bytes_recv = recv(sockfd, vec.data() + total_received, 
                         size - total_received, 0);
        if (bytes_recv <= 0) {
            throw std::runtime_error("Connection closed or error");
        }
        total_received += bytes_recv;
    }
    
    // Write to file
    std::ofstream file(filename, std::ios::binary); 
    file.write(vec.data(), vec.size()); 
    file.close(); 
    
    std::cout << "WRITE TO " << filename << " SUCCESSFUL" << std::endl;
}
