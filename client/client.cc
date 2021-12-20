#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <random>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include <random>

#include "mock_message_board.grpc.pb.h"
#include "clientAsync.h"
#include "../shared/consts.h"


clientAsync::clientAsync(const std::shared_ptr<Channel> &channel, std::shared_ptr<ServerStats2> serverStats)
        : stub_(messageService::NewStub(channel)), serverStats(std::move(serverStats)) {}

void clientAsync::findLastMessage(const std::string &cliend_id, uint64_t query_uid) {
    // Data we are sending to the server.
    findLastMessageRequest request;
    request.set_client_id(cliend_id);
    request.set_query_uid(query_uid);

    // Call object to store rpc data
    auto *call = new AsC_findLastMessageCall(serverStats);

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->response_reader = stub_->PrepareAsyncfindLastMessage(&call->context, request, &cq_);

    // StartCall initiates the RPC call
    call->response_reader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->response_reader->Finish(&call->reply, &call->status, (void *) call);
}

void clientAsync::sendMessage(const std::string &cliend_id, const std::string &message, uint64_t query_uid) {
    // Data we are sending to the server.
    saveMessageRequest request;
    request.set_client_id(cliend_id);
    request.set_message(message);
    request.set_query_uid(query_uid);

    // Call object to store rpc data
    auto *call = new AsC_saveMessageCall(serverStats);

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->response_reader = stub_->PrepareAsyncsendMessage(&call->context, request, &cq_);

    // StartCall initiates the RPC call
    call->response_reader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->response_reader->Finish(&call->reply, &call->status, (void *) call);
}

void clientAsync::AsyncCompleteRpc() {
    void *tag;
    bool ok = false;

    // Block until the next result is available in the completion queue "cq".
    while (true) {
        GPR_ASSERT(cq_.Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<asyncHandler *>(tag)->Proceed(ok);
    }
}

int main(int argc, char **argv) {

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
    

    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint (in this case,
    // localhost at port 50051). We indicate that the channel isn't authenticated
    // (use of InsecureChannelCredentials()).
    std::shared_ptr<ServerStats2> serverStats = std::make_shared<ServerStats2>(STATS_FILES_DIR CLIENT_SYNC_FILENAME);
    clientAsync client(grpc::CreateChannel(M_MESSAGE_SERVICE_SYNC_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()),
                       serverStats);
    uint64_t client_uid = generate_local_uid();

    // Spawn reader thread that loops indefinitely
    // only calls findmessage atm
    std::thread thread_ = std::thread(&clientAsync::AsyncCompleteRpc, &client);
    std::thread thread2_ = std::thread(&clientAsync::AsyncCompleteRpc, &client);
    
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

    std::cout << "Press control-c to quit" << std::endl << std::endl;
    thread_.join();  // blocks forever
    thread2_.join();  // blocks forever

    return 0;
}

