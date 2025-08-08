#include "/home/shark/code/SmallStore/include/clientclass.h"
int main() {

	SmallStoreClient db;
	db.SmallClientStart();
	db.sendReq("PUT HASHMAP key1\n"); 
	db.sendReq("PUTIN HASHMAP key1 passwords 1.23456\n");
	db.sendReq("GET HASHMAP key1 passwords\n");
	db.sendReq("SAVE\n");
	float result = db.recvIntoFloat(); 
	float result2 = result - 0.03456;
	std::cout<<"\nGot:"<<result<<"\nAlso\n"<<result2<<std::endl; 


}
