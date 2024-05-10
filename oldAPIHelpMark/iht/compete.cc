#include <protos/workloaddriver.pb.h>
#include <vector>

#include "../logging/logging.h"
#include "../vendor/sss/cli.h"

#include "common.h"
#include "experiment.h"
#include "role_client.h"
#include "role_server.h"
#include "rpc.h"

auto ARGS = {
    sss::I64_ARG("--node_id", "The node's id. (nodeX in cloudlab should have X in this option)"),
    // sss::I64_ARG("--runtime", "How long to run the experiment for. Only valid if unlimited_stream"),
    // sss::BOOL_ARG_OPT("--unlimited_stream", "If the stream should be endless, stopping after runtime"),
    // sss::I64_ARG("--op_count", "How many operations to run. Only valid if not unlimited_stream"),
    // sss::I64_ARG("--region_size", "How big the region should be in 2^x bytes"),
    sss::I64_ARG("--thread_count", "How many threads to spawn with the operations"),
    sss::I64_ARG("--node_count", "How many nodes are in the experiment"),
    // sss::I64_ARG("--qp_max", "The max number of queue pairs to allocate for the experiment."),
    // sss::I64_ARG("--contains", "Percentage of operations are contains, (contains + insert + remove = 100)"),
    // sss::I64_ARG("--insert", "Percentage of operations are inserts, (contains + insert + remove = 100)"),
    // sss::I64_ARG("--remove", "Percentage of operations are removes, (contains + insert + remove = 100)"),
    // sss::I64_ARG("--key_lb", "The lower limit of the key range for operations"),
    // sss::I64_ARG("--key_ub", "The upper limit of the key range for operations"),
    sss::I64_ARG_OPT("--cache_depth", "The depth of the cache for the IHT", 0),
    sss::BOOL_ARG_OPT("--server", "If this node should send or receive data..."),

};

#define PATH_MAX 4096
#define PORT_NUM 18000

using namespace rome::rdma;

// The optimial number of memory pools is mp=min(t, MAX_QP/n) where n is the number of nodes and t is the number of threads
// To distribute mp (memory pools) across t threads, it is best for t/mp to be a whole number
// IHT RDMA MINIMAL

// Copied from memory pool, cm initialization should be apart of cm class, not in memory pool
sss::StatusVal<unordered_map<int, Connection*>> init_cm(ConnectionManager* cm, Peer self, const vector<Peer>& peers){
    auto status = cm->Start(self.address, self.port);
    RETURN_STATUSVAL_FROM_ERROR(status);
    ROME_INFO("Starting with {}", self.address);
    // Go through the list of peers and connect to each of them
    for (const auto &p : peers) {
        ROME_INFO("Init with {}", p.address);
        auto connected = cm->Connect(p.id, p.address, p.port);
        while (connected.status.t == sss::Unavailable) {
            connected = cm->Connect(p.id, p.address, p.port);
        }
        RETURN_STATUSVAL_FROM_ERROR(connected.status);
        ROME_INFO("Init done with {}", p.address);
    }

    // Test out the connection (receive)
    AckProto rm_proto;
    for (const auto &p : peers) {
      auto conn = cm->GetConnection(p.id);
      STATUSVAL_OR_DIE(conn);
      status = conn.val.value()->channel()->Send(rm_proto);
      RETURN_STATUSVAL_FROM_ERROR(status);
    }

    unordered_map<int, Connection*> connections;

    // Test out the connection (deliver)
    for (const auto &p : peers) {
      auto conn = cm->GetConnection(p.id);
      STATUSVAL_OR_DIE(conn);
      auto got =
          conn.val.value()->channel()->template Deliver<AckProto>();
      RETURN_STATUSVAL_FROM_ERROR(got.status);
      connections[p.id] = conn.val.value();
    }
    return {sss::Status::Ok(), connections};
}

int main(int argc, char **argv) {
    ROME_INIT_LOG();
    ROME_INFO("Running twosided");

    sss::ArgMap args;
    // import_args will validate that the newly added args don't conflict with
    // those already added.
    ROME_INFO("args obj");
    auto res = args.import_args(ARGS);
    if (res) {
        ROME_ERROR(res.value());
        exit(1);
    }
    ROME_INFO("imported args?");
    // NB: Only call parse_args once.  If it fails, a mandatory arg was skipped
    res = args.parse_args(argc, argv);
    if (res) {
        args.usage();
        ROME_ERROR(res.value());
        exit(1);
    }
    ROME_INFO("parsed");

    // Extract the args to variables
    //BenchmarkParams params =  BenchmarkParams(args);
    ROME_INFO("Running IHT with cache depth 0");

    // Check node count
    if (args.iget("--node_count") <= 0 || args.iget("--thread_count") <= 0){
        ROME_INFO("Cannot start experiment. Node/thread count was found to be 0");
        exit(1);
    }
    // Check we are in this experiment
    if (args.iget("--node_id") >= args.iget("--node_count")){
        ROME_INFO("Not in this experiment. Exiting");
        exit(0);
    }

    // Start initializing a vector of peers
    // We have separate versions for the vector (with only different in ports used) in order to create two connection managers
    std::vector<Peer> recvs;
    std::vector<Peer> sends;
    Peer self_sender;
    Peer self_receiver;
    for(uint16_t n = 0; n < args.iget("--node_count"); n++){
        // Create the ip_peer (really just node name)
        std::string ippeer = "node";
        std::string node_id = std::to_string(n);
        ippeer.append(node_id);
        // Create the peer and add it to the list
        Peer send_next = Peer(n, ippeer, PORT_NUM + n + 1);
        Peer recv_next = Peer(n, ippeer, PORT_NUM + n + 1001);
        if (n == args.iget("--node_id")) {
            self_sender = send_next;
            self_receiver = recv_next;
        }
        sends.push_back(send_next);
        recvs.push_back(recv_next);
    }
    Peer host = sends.at(0); // portnum doesn't matter so we can get either from sends or recvs

    
    ConnectionManager* sender = new ConnectionManager(self_sender.id);
    ConnectionManager* receiver = new ConnectionManager(self_receiver.id);
    sss::StatusVal<unordered_map<int, Connection*>> s1 = init_cm(sender, self_sender, sends);
    ROME_ASSERT(s1.status.t == sss::Ok, "Connection manager 1 was setup incorrectly");
    unordered_map<int, Connection*> sender_map = s1.val.value();
    sss::StatusVal<unordered_map<int, Connection*>> s2 = init_cm(receiver, self_receiver, recvs);
    ROME_ASSERT(s2.status.t == sss::Ok, "Connection manager 2 was setup incorrectly");
    unordered_map<int, Connection*> receiver_map = s2.val.value();
    ROME_INFO("Init 2 cms!");


    if(args.bget("--server")){
        ROME_INFO("started server track");
        bool listen = true;
        while (listen)
        {
            for(int id = 0; id < args.iget("--node_count"); id++){
                if (id == args.iget("--node_id")) continue; // skip my id
                // Try to get a value
                std::optional<IHTOPProto> maybe_req = sender_map[id]->channel()->TryReceive<IHTOPProto>();
                // value here referes to the optional and not the IHTOpProto itself...
                if (!maybe_req.has_value()){
                    //ROME_INFO("nope");
                    continue;
                }
                ROME_INFO("receive had value! ");
                IHTOPProto request = maybe_req.value();
                auto op = request.op_type();
                if (op == GET_RES){
                    ROME_INFO("got get_res from rdma connection");
                }
                ROME_INFO("got {} from the key", request.key());
                listen = false;
                
            }
        }
    }

    if(!args.bget("--server")){
        IHTOPProto response;
        response.set_op_type(GET_RES);
        response.set_key(33);
        response.set_value(0);
        // Send the response
        int id = 0;
        sss::Status stat = sender_map[id]->channel()->Send(response);
        ROME_ASSERT(stat.t == sss::Ok, "Operation failed");
    }
}
