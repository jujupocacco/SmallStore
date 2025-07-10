# A Simple In-Memory DB built as a learning project.

## Compiling the server:
  To compile the server, host.cpp, persistence.cpp, and functions.cpp need to be linked together.
## Sending requests:
  To send requests from a project, compile your project with the dedicated client file. The request format uses "\n" as a delimiter for each requests, without it, the parsing will not function as intended. 
  From your project, create a "TinyDBClient" object to use the functions associated. On the client side there are only two functions needed. Those being TinyClientStart(), and sendReq(). The "sendReq" function takes a range from 1 to 5 substrings. 

## Example usage:

  #include "/home/user/TinyDB/include/client.h" 
  #include <iostream>
 
  
  int main(){
  
    TinyDBClient db;    
    db.sendReq("PUT HASHMAP key1"); // Initializes an empty nested unordered_map
    db.sendReq("PUTIN HASHMAP key1 x y"); // Creates a field in the hashmap "x" with a field named "y"
    db.sendReq("GET HASHMAP key1 x"); // Returns the value at field
    std::string result = db.recvIntoStr(); // Recieves the value "y" into a std::string "result"
    
  }
