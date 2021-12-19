//
// Created by adrien on 19.12.21.
//

#include <random>
#include "client.h"
#include "../shared/consts.h"

using namespace std;

int main(int argc, char** argv) {

    int i = 0;
    uint32_t upperBound = ((argc > ++i) ? stoi(argv[i]) : 256);
    uint32_t meanWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 1000);
    uint32_t stdWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 500);
    float findRequestProportion = ((argc > ++i) ? stof(argv[i]) : 0.5);

    mt19937 generator;
    normal_distribution<float> normal_dist(meanWaitingTime, stdWaitingTime);
    uniform_real_distribution<float> uni_dist(0.f, 1.f);
    if (argc > ++i)
        generator.seed(stoi(argv[i]));

    std::shared_ptr<ServerStats2> serverStats = std::make_shared<ServerStats2>(STATS_FILES_DIR CLIENT_SYNC_FILENAME);
    Client client(grpc::CreateChannel(M_MESSAGE_SERVICE_SYNC_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()));
    uint64_t client_uid = generate_local_uid();

    for (int i = 1; i < upperBound; i++) {
        uint64_t query_uid = get_query_uid(client_uid, i);
        serverStats->add_entry(query_uid, get_epoch_time_us());
        
        float p = uni_dist(generator);
        if (p < findRequestProportion){
            // cout << "**FIND**" << i << "\n";
            client.findLastMessage("admin", query_uid);  // The actual RPC call!
        } else{
            // cout << "->SEND<- " << i << "\n";
            client.sendMessage("admin", "world " + to_string(i), query_uid);  // The actual RPC call!
        }
        
        this_thread::sleep_for(std::chrono::microseconds(static_cast<long>((normal_dist(generator)))));
    }

    return 0;
}