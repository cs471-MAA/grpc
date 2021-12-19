//
// Created by adrien on 18.12.21.
//

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "mock_message_board.grpc.pb.h"
#include <cinttypes>
#include "../shared/asyncHandler.h"


using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using mmb::messageService;
using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;

class AsyncClient {
public:
    explicit AsyncClient(const std::shared_ptr<Channel> &channel);

    void findLastMessage(const std::string& cliend_id, uint64_t query_uid = 0);
    void sendMessage(const std::string& cliend_id, const std::string& message, uint64_t query_uid = 0);

    /** Loop while listening for completed responses.
     ** Prints out the response from the server.
     **/
    void AsyncCompleteRpc();

private:
    class AsC_findLastMessageCall : public asyncHandler {
    public:
        // Container for the data we expect from the server.
        findLastMessageReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // Storage for the status of the RPC upon completion.
        Status status;

        std::unique_ptr<ClientAsyncResponseReader<findLastMessageReply>> response_reader;

        void Proceed(bool ok) override{
            if(ok){
                std::cout << "findMessage: " << reply.message() << std::endl;
            }else{
                std::cout << "findMessage Error: " << status.error_details() << std::endl;
            }
        }
    };

    // struct for keeping state and data information
    class AsC_saveMessageCall : public asyncHandler {
    public:
        // Container for the data we expect from the server.
        saveMessageReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // Storage for the status of the RPC upon completion.
        Status status;

        std::unique_ptr<ClientAsyncResponseReader<saveMessageReply>> response_reader;

        void Proceed(bool ok) override{
            if(!ok){
                std::cout << "saveMessage Error: " << status.error_details() << std::endl;
            }
        }
    };

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<messageService::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq_;
};
