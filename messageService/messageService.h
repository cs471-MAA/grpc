#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include <sstream>
#include <utility>

#include "mock_message_board.grpc.pb.h"


using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using mmb::messageService;
using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;


// Class encompasing the state and logic needed to serve a request.
class CallData {
public:
    friend class M_asyncClient;
    /** Take in the "service" instance (in this case representing an asynchronous
    * server) and the completion queue "cq" used for asynchronous communication
    * with the gRPC runtime.
     */
    CallData(messageService::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<grpc::Channel>  channel,
             grpc::CompletionQueue *cqClient);

    void Proceed();

private:
    void Finish(const std::string &response);

    void FinishWithError();

    grpc::CompletionQueue *cqClient;

    const std::shared_ptr<grpc::Channel> channel;
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    messageService::AsyncService *service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue *cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    findLastMessageRequest request_;
    // What we send back to the client.
    findLastMessageReply reply_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<findLastMessageReply> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus {PROCESS, FINISH};
    std::atomic<CallStatus> status_;  // The current serving state.
};
