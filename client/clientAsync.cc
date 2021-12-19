#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

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
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint (in this case,
    // localhost at port 50051). We indicate that the channel isn't authenticated
    // (use of InsecureChannelCredentials()).

    std::shared_ptr<ServerStats2> serverStats = std::make_shared<ServerStats2>(STATS_FILES_DIR "clientAsync.csv");
    clientAsync client(grpc::CreateChannel(M_MESSAGE_SERVICE_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()),
                       serverStats);


    // Spawn reader thread that loops indefinitely
    // only calls findmessage atm
    std::thread thread_ = std::thread(&clientAsync::AsyncCompleteRpc, &client);
    std::thread thread2_ = std::thread(&clientAsync::AsyncCompleteRpc, &client);

    int upper_bound = 10000;
    for (int i = 1; i < upper_bound; i++) {
        std::string user("world " + std::to_string(i));
        serverStats->add_entry(i, get_epoch_time_us());
        client.findLastMessage("world " + std::to_string(i - 40), i);  // The actual RPC call!

        int second_request = upper_bound + i;
        serverStats->add_entry(second_request, get_epoch_time_us());
        client.sendMessage("world " + std::to_string(i), "world " + std::to_string(i),
                           second_request);  // The actual RPC call!
    }

    std::cout << "Press control-c to quit" << std::endl << std::endl;
    thread_.join();  // blocks forever
    thread2_.join();  // blocks forever

    return 0;
}

