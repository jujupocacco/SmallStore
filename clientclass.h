#pragma once 
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <netinet/tcp.h>
#include <iostream> 
#include <stdexcept>
#include <cstring>
#include <vector>
#include <fstream> 

class SmallStoreClient{
	private: 
		int sockfd; 
		bool connected;

		int createSocket(); 
		sockaddr_in serverStructure(); 
		bool connectionHandling(sockaddr_in serveraddr); 	

	public: 
		SmallStoreClient();
		~SmallStoreClient(); 
		void SmallClientStart(); 
		void sendReq(const std::string& req); 
		std::string recvIntoStr();
		int recvIntoInt(); 
		float recvIntoFloat(); 
		double recvIntoDouble(); 
		void recvIntoBin(std::string filename); 

};
