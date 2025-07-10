#pragma once
#include <ios> 
#include <iostream>
#include <unordered_map>
#include <string> 
#include <vector> 
#include <json.hpp>
#include <fstream>
#include <cstdint> 
#include <cstdio>
#include <netinet/tcp.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <iostream> 
#include <stdexcept>
#include <cstring>
#include <sstream> 
#include <cstdlib>
#include <chrono> 
#include <thread>
#include <shared_mutex>
#include <algorithm>
#include <utility> 
#include <set> 
// ADD SUPPORT FOR OTHER TYPES
// START WORKING ON CACHING

class Database {

private:
	std::unordered_map<std::string, std::set<std::string>> sets;  
	std::unordered_map<std::string, std::string> strings;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> hashmaps;
	std::unordered_map<std::string, std::vector<std::string>> arrays;
	std::unordered_map<std::string, std::string> key_types;
	std::unordered_map<std::string, std::vector<char>> execs;
	std::unordered_map<std::string, std::vector<std::string>> bins;
	std::unordered_map<std::string, int> access;

public: 

   	void putString(const std::string& key);

	void putSet(const std::string& key);
	void clearSet(const std::string& key);  
	void insertSet(const std::string& key, const std::string& value);

   	void fillString(const std::string& key, const std::string& value);
    	void putArray(const std::string& key);
	void putInArray(const std::string& key, const int& index, const std::string& value);
	void appendToArray(const std::string& key, const std::string& value);
	
   	void putHashmap(const std::string& key);
   	void putInHashmap(const std::string& key, const std::string& field, const std::string& value);
	void putBinary(const std::string& key, const std::string& srcfilepath, const std::string& dstfilepath);
	
	bool remove(const std::string& key);
	void removeArrayElement(const std::string& key, const int& index);
	void removeSetElement(const std::string& key, const std::string& value);
	void removeStringElement(const std::string& key, const int& index);

	std::string getString(const std::string& key, const std::string& field, const int& index);
	std::vector<char> getBinary(const std::string& key); 

	void sendToLRUCache(); 
	void evictFromLRUCache(); 
	void saveToJSON();
	void loadFromJSON();
	void periodicalSaveToJSON(); 


};

