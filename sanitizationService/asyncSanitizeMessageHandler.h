
#include "mock_message_board.grpc.pb.h"
#include "../shared/asyncHandler.h"
#include "../shared/thread_pool.h"
#include "../shared/ServerStats2.h"

using mmb::sanitizationService;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerAsyncResponseWriter;
using grpc::Status;

// Break circular dependency
class asyncSanitizeMessageHandler;

class mockDatabase_asyncClient : public asyncHandler {
public:
    explicit mockDatabase_asyncClient(const std::shared_ptr<grpc::ChannelInterface> &channel, grpc::CompletionQueue *cq,
                                      asyncSanitizeMessageHandler *callData);

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    void saveMessage(const saveMessageRequest &request);

    // Will be called by the HandleChannel thread because of the cq_ variable this class use is the one that's looped on
    // in the HandleChannel function
    void Proceed(bool ok) override;

private:
    // Everything has to be declared as class variable because of pointer magic that will make code from the HandleChannel
    // thread write stuff in this class (with Proceed()) that was created by the HandleRPC thread

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<mmb::mockDatabase::Stub> stub_;
    grpc::CompletionQueue *cq_{};

    // Container for the data we expect from the server.
    saveMessageReply reply;
    // Storage for the status of the RPC upon completion.
    Status status;
    std::unique_ptr<grpc::ClientAsyncResponseReader<saveMessageReply>> rpc;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // The means to get back to the client.
    asyncSanitizeMessageHandler *callData_;
};

// ###############################################

// Class encompasing the state and logic needed to serve a request.
class asyncSanitizeMessageHandler : public asyncHandler {
public:
    friend class mockDatabase_asyncClient;

    /** Take in the "service" instance (in this case representing an asynchronous
    * server) and the completion queue "cq" used for asynchronous communication
    * with the gRPC runtime.
     */
    asyncSanitizeMessageHandler(sanitizationService::AsyncService *service, ServerCompletionQueue *cq,
                                std::shared_ptr<grpc::ChannelInterface> channel, grpc::CompletionQueue *cqClient,
                                thread_pool &threadPool, std::chrono::microseconds waiting_time,
                                std::shared_ptr<ServerStats2> serverStats);

    void Proceed(bool ok) override;

private:
    void Finish(const saveMessageReply &reply);

    void FinishWithError();

    grpc::CompletionQueue *cqClient;

    const std::shared_ptr<grpc::ChannelInterface> channel;
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    sanitizationService::AsyncService *service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue *cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    saveMessageRequest request_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<saveMessageReply> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus {
        PROCESS, FINISH
    };
    std::atomic<CallStatus> status_;  // The current serving state.

    thread_pool &threadPool;
    std::chrono::microseconds waiting_time;
    std::shared_ptr<ServerStats2> serverStats;
};