#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "mock_message_board.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using mmb::messageService;
using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;

class Client {
public:
    Client(std::shared_ptr<Channel> channel): stub_(mmb::messageService::NewStub(channel)) {}

    std::string findLastMessage(const std::string &cliend_id, uint64_t query_uid = 0){
        // Data we are sending to the server.
        findLastMessageRequest request;
        request.set_client_id(cliend_id);
        request.set_query_uid(query_uid);

        // Container for the data we expect from the server.
        findLastMessageReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->findLastMessage(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    bool sendMessage(const std::string &cliend_id, const std::string &message, uint64_t query_uid = 0){
        // Data we are sending to the server.
        saveMessageRequest request;
        request.set_client_id(cliend_id);
        request.set_message(message);
        request.set_query_uid(query_uid);

        // Container for the data we expect from the server.
        saveMessageReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->sendMessage(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            return reply.ok();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<mmb::messageService::Stub> stub_;
};


