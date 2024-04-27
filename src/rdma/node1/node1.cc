// https://www.geeksforgeeks.org/socket-programming-in-cpp/
// C++ program to show the example of server application in 
// socket programming 
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <google/protobuf/message_lite.h> 
#include "../../Rabia_build/message.pb.h"
//using namespace google::protobuf;
  
//using namespace std; 
// namespace google::protobuf;
  
int main() 
{ 
  // creating socket 
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 

  // specifying the address 
  sockaddr_in serverAddress; 
  serverAddress.sin_family = AF_INET; 
  serverAddress.sin_port = htons(8080); 
  serverAddress.sin_addr.s_addr = INADDR_ANY; 

  // binding socket. 
  bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)); 

  message::Msg sample_message;

  // listening to the assigned socket 
  listen(serverSocket, 5); 

  // accepting connection request 
  int clientSocket = accept(serverSocket, nullptr, nullptr); 

  // recieving data 
  char buffer[1024] = { 0 }; 
  recv(clientSocket, buffer, sizeof(buffer), 0); 
  std::string data(buffer);
  sample_message.ParseFromString(buffer);//couldnt figure out how to get thsi linked :(
  std::cout << "Message from client recieved: \n" << sample_message.value() << std::endl; 

  // closing the socket. 
  close(serverSocket); 

  return 0; 
}