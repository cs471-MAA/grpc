#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include <sstream>
#include <utility>

#include "mock_message_board.grpc.pb.h"
#include "messageService.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using mmb::mockDatabase;
using mmb::sanitizationService;
using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;

class M_asyncClient {
public:
    explicit M_asyncClient(const std::shared_ptr<grpc::Channel> &channel, grpc::CompletionQueue *cq,
                                CallData *callData);

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    void findLastMessage(const std::string &cliend_id);

    // Will be called by the HandleChannel thread because of the cq_ variable this class use is the one that's looped on
    // in the HandleChannel function
    void handleCallback();

private:
    // Everything has to be declared as class variable because of pointer magic that will make code from the HandleChannel
    // thread write stuff in this class (with handleCallback()) that was created by the HandleRPC thread

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<mockDatabase::Stub> stub_;
    grpc::CompletionQueue *cq_{};

    // Container for the data we expect from the server.
    findLastMessageReply reply;
    // Storage for the status of the RPC upon completion.
    Status status;
    std::unique_ptr<grpc::ClientAsyncResponseReader<findLastMessageReply>> rpc;
    findLastMessageRequest request;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // The means to get back to the client.
    CallData *callData_;
};
