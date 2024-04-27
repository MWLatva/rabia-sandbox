// https://www.geeksforgeeks.org/socket-programming-in-cpp/
// C++ program to illustrate the client application in the 
// socket programming 
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include "../../Rabia_build/message.pb.h"
  
int main() 
{ 
    // creating socket 
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0); 
  
    // specifying address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 

    message::Msg sample_message;
    //message::ConsensusObj consObj;

    sample_message.set_type(message::MsgType::Proposal);
    sample_message.set_phase(1);
    sample_message.set_value(5000);
    //sample_message.set_allocated_obj(consObj);

  
    // sending connection request 
    connect(clientSocket, (struct sockaddr*)&serverAddress, 
            sizeof(serverAddress)); 
  
    // sending data 
    std::string message;
    sample_message.SerializeToString(&message); //protobuf serialise
    send(clientSocket, message.c_str(), strlen(message.c_str()), 0); 
    
  
    // closing socket 
    close(clientSocket); 
  
    return 0; 
}