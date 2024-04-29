#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include "../../Rabia_build/message.pb.h"

#include <remus/rdma/connection_map.h>
#include <remus/util/status.h>
//#include <rome/rdma/connection_manager/connection_manager.h>

#include <atomic>
#include <thread>
#include <unordered_map>

using namespace std;
using namespace remus::rdma::internal;
using namespace remus::util;
using namespace google::protobuf;
  

struct Peer {
  const uint16_t id = 0;          // A unique Id for the peer
  const std::string address = ""; // The public address of the peer
  const uint16_t port = 0;        // The port on which the peer listens

  /// Construct a Peer
  ///
  /// TODO: We probably don't need a constructor since this is so trivial
  ///
  /// @param id       The Id for the peer (default 0)
  /// @param address  The ip address as a string (default "")
  /// @param port     The port (default 0)
  Peer(uint16_t id = 0, std::string address = "", uint16_t port = 0)
      : id(id), address(address), port(port) {}
};

int main() {      
    //thanks Ethan!
    ROME_INIT_LOG();
    ROME_INFO("CREATING PEER");
    //create a peer
    std::string node_name = "node0";
    Peer self_peer = Peer(0, node_name, 18000);
    Peer other_peer = Peer(1, "note1", 18001);

    ConnectionManager* cm = new ConnectionManager(self_peer.id);

    //start a connection manager on the 'self' peer
    auto status = cm->Start(self_peer.address, self_peer.port);
    RETURN_STATUSVAL_ON_ERROR(status);

    // connect self peer to cm
    auto connected = cm->Connect(self_peer.id, self_peer.address, self_peer.port);
    while (connected.status.t == Unavailable) {
        connected = cm->Connect(self_peer.id, self_peer.address, self_peer.port);
    }
    RETURN_STATUSVAL_ON_ERROR(connected.status);
    ROME_INFO("Init done with {}", self_peer.address);

    //connect other peer to cm
    auto connected = cm->Connect(other_peer.id, other_peer.address, other_peer.port);
    while (connected.status.t == Unavailable) {
        connected = cm->Connect(other_peer.id, other_peer.address, other_peer.port);
    }
    RETURN_STATUSVAL_ON_ERROR(connected.status);
    ROME_INFO("Init done with {}", other_peer.address);

    //__________________

    message::Msg sample_message;

    sample_message.set_type(message::MsgType::Proposal);
    sample_message.set_phase(1);
    sample_message.set_value(5000);
    //sample_message.set_allocated_obj(consObj);

  
    // // sending data 
    // std::string message;
    // sample_message.SerializeToString(&message); //protobuf serialise

    auto conn = cm->GetConnection(1);
    Status stat = conn.val.value()->channel()->Send(sample_message);
    REMUS_ASSERT(stat.t == Ok, "Operation failed"); 
    
  
  
    return 0; 
}