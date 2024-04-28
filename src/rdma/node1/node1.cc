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

#include <remus/rdma/connection_map.h>
#include <remus/util/status.h>

#include <atomic>
#include <thread>
#include <unordered_map>

using namespace std;
using namespace remus::rdma::internal;
using namespace remus::util;
using namespace google::protobuf;
  
int main() 
{ 
 
  // make the connection object


  Connection* sender;
  Connection* receiver;

  message::Msg sample_message;

  /// Receive the result
  StatusVal<Msg> maybe_result = sender->channel()->Deliver<Msg>();
  lock_table_client[target_id]->unlock();
  REMUS_ASSERT(maybe_result.status.t == Ok, "Cannot get result");
  Msg result = maybe_result.val.value();

  std::cout << "Message from client recieved: \n" << result.value() << std::endl; 

  return 0; 
}