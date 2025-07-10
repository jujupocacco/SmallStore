#include "/home/shark/code/TinyDB/include/TinyDB.h"

void Database::putString(const std::string& key){
	access[key]+=1;
	strings[key] = "";
	key_types[key] = "string"; 
}
void Database::putSet(const std::string& key){
	access[key]+=1;
	sets[key] = std::set<std::string>(); 
	key_types[key] = "set"; 
} 
void Database::clearSet(const std::string& key){
	access[key] += 1;
	auto it = sets.find(key);  
	if (it != sets.end()){
		it->second.clear(); 
	}  
}
void Database::insertSet(const std::string& key, const std::string& value){
	access[key] += 1;
	auto it = sets.find(key);
	if (it != sets.end()){
		it->second.insert(value); 
	} 
}  
void Database::fillString(const std::string& key, const std::string& value){
	access[key]+=1;
	if (strings.find(key) != strings.end()){
		strings[key] = value;
	}
	else{
		throw std::runtime_error("KEY DOES NOT EXIST"); 
	}

}
void Database::putArray(const std::string& key){
	access[key]+=1;
	arrays[key]=std::vector<std::string>{};
	key_types[key] = "array";
}
void Database::putInArray(const std::string& key, const int& index, const std::string& value){
	access[key]+=1;
	if (arrays.find(key) != arrays.end()){
		if(arrays[key].size()<=index){
			arrays[key].resize(index + 1); 
		} 
		arrays[key][index] = value; 
	}
	else{
		throw std::runtime_error("KEY DOES NOT EXIST");
	}
} 
void Database::appendToArray(const std::string& key, const std::string& value){
	access[key]+=1;
	if (arrays.find(key) != arrays.end()){ 
		arrays[key].push_back(value);
	}
	else{
		throw std::runtime_error("KEY DOES NOT EXIST");
	}
} 
void Database::putHashmap(const std::string& key){
	access[key]+=1;
	hashmaps[key] = std::unordered_map<std::string, std::string> {}; 
	key_types[key] = "hashmap";
}
void Database::putInHashmap(const std::string& key, const std::string& field, const std::string& value){
	access[key]+=1;
	if (hashmaps.find(key) != hashmaps.end()){
		hashmaps[key][field] = value; 
		key_types[key] = "hashmap";
	}
	else{
		throw std::runtime_error("KEY DOES NOT EXIST");
	}
} 	

void Database::putBinary(const std::string& key, const std::string& srcfilepath, const std::string& dstfilepath){

	access[key]+=1;
	key_types[key] = "binary";
	std::ifstream binfile(srcfilepath, std::ios::binary);
	std::ofstream storebinfile(dstfilepath, std::ios::binary); 
	storebinfile << binfile.rdbuf(); 
	binfile.close();
	storebinfile.close();

	bins[key].push_back(dstfilepath);

} 

std::string Database::getString(const std::string& key, const std::string& field, const int& index){
	access[key]+=1;
	auto it = key_types.find(key);
	if (it != key_types.end()){
		if (it -> second == "string"){
			return strings[key];
		}
		else if (it -> second == "hashmap"){
			return hashmaps[key][field];
		}
		else if (it -> second == "array"){
			return arrays[key][index];
		}
	}
	return "";
} 
std::vector<char> Database::getBinary(const std::string& key){
	access[key]+=1;
	auto it = bins.find(key);
	if(it!=bins.end()){
		auto filepath = it->second.at(0); 
		std::ifstream file(filepath, std::ios::binary);
		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<char> buffer(size);
		file.read(buffer.data(), size);

		return buffer;
	}
	return std::vector<char>(); 
} 
bool Database::remove(const std::string& key){ 

	access[key]+=1;
	auto typeFind = key_types.find(key);

	if (typeFind!=key_types.end()){

		std::string type = typeFind->second;

		if(type == "hashmap"){
			hashmaps.erase(key);
		}
		else if(type == "string"){
			strings.erase(key);
		}
		else if(type == "array"){
			arrays.erase(key);
		} 
	
		key_types.erase(key); 
		return true; 
	}
	return false;
}
void Database::removeArrayElement(const std::string& key, const int& index){
	access[key]+=1;
	arrays[key].erase(arrays[key].begin() + index);

}
void Database::removeStringElement(const std::string& key, const int& index){
	access[key]+=1;
	strings[key].erase(strings[key].begin() + index);
}

void Database::sendToLRUCache(){
	
} 
void Database::evictFromLRUCache(){

	std::chrono::seconds cacheTime(10); 
	sleep(cacheTime.count());
	std::vector<int> ktr;

	auto it = access.begin(); 
	while (it != access.end()){
		ktr.push_back(it -> second);
		int min = ktr[0];
		for(int i = 0; i>ktr.size(); i++){
			if (ktr[i] < min){
				min = ktr[i];
			}
		} 
		strings.erase(it->first);
		hashmaps.erase(it->first);
		arrays.erase(it->first);
		bins.erase(it->first);
		key_types.erase(it->first);
		it = access.erase(it);

		std::cout<<"CACHED\n";
	}
}
