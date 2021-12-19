#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "mock_message_board.grpc.pb.h"
#include "../shared/thread_pool.h"
#include "../shared/asyncHandler.h"

using grpc::Status;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using mmb::mockDatabase;
using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;

// Class encompasing the state and logic needed to serve a request.
class asyncFindLastMessageHandler : public asyncHandler {
public:
    // Take in the "service" instance (in this case representing an asynchronous
    // server) and the completion queue "cq" used for asynchronous communication
    // with the gRPC runtime.
    asyncFindLastMessageHandler(mockDatabase::AsyncService* service, ServerCompletionQueue* cq, thread_pool &threadPool,
                                std::chrono::microseconds waiting_time, CTSL::HashMap<std::string, std::string> &hashMap)
            : service_(service), cq_(cq), responder_(&ctx_), status_(PROCESS), threadPool(threadPool),
              waiting_time(waiting_time), hashMap(hashMap){

        // As part of the initial CREATE state, we *request* that the system
        // start processing SayHello requests. In this request, "this" acts are
        // the tag uniquely identifying the request (so that different CallData
        // instances can serve different requests concurrently), in this case
        // the memory address of this CallData instance.
        service_->RequestfindLastMessage(&ctx_, &request_, &responder_, cq_,
                                         cq_,this);
    }

    void Proceed(bool ok) override {
        if (status_ == PROCESS) {

            // Push the request into a worker thread pool just like a real DB would do
            threadPool.push_task([&] (){

                // The actual processing.
                reply_.set_query_uid(request_.query_uid());
                std::string result;
                if(hashMap.find(request_.client_id(), result)){
                    reply_.set_message( result);
                }else{
                    reply_.set_message( "Client ID not found");
                }

                status_ = FINISH;
                std::this_thread::sleep_for(waiting_time);
                responder_.Finish(reply_, Status::OK, this);
            });

            // Spawn a new CallData instance to serve new clients while we process
            // the one for this CallData. The instance will deallocate itself as
            // part of its FINISH state.
            new asyncFindLastMessageHandler(service_, cq_, threadPool, waiting_time, hashMap);
        } else {
            GPR_ASSERT(status_ == FINISH);
            // Once in the FINISH state, deallocate ourselves (CallData).
            delete this;
        }
    }

private:
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    mockDatabase::AsyncService* service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue* cq_;
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
    enum CallStatus { PROCESS, FINISH };
    std::atomic<CallStatus> status_; // The current serving state.

    thread_pool& threadPool;
    std::chrono::microseconds waiting_time;
    CTSL::HashMap<std::string, std::string> &hashMap;
};
