

#include "messageService.h"
#include "asyncClient.h"
#include <utility>

CallData::CallData(messageService::AsyncService *service, ServerCompletionQueue *cq,
                   std::shared_ptr<grpc::Channel>  channel, grpc::CompletionQueue *cqClient)
        : service_(service), cq_(cq), responder_(&ctx_), status_(PROCESS), cqClient(cqClient), channel(std::move(channel)) {

    // As part of the initial CREATE state, we *request* that the system
    // start processing SayHello requests. In this request, "this" acts are
    // the tag uniquely identifying the request (so that different CallData
    // instances can serve different requests concurrently), in this case
    // the memory address of this CallData instance.
    service_->RequestfindLastMessage(&ctx_, &request_, &responder_, cq_, cq_,
                                     this);
}

void CallData::Proceed() {
    if (status_ == PROCESS) {
        std::stringstream ss;
        ss << std::this_thread::get_id();

        // The actual processing.
        std::string prefix("Hello from server1 threadID " + ss.str());
        //std::string bidule = greeter.SayHello("server1");

        auto asyncClient = new M_asyncClient(channel, cqClient, this);
        asyncClient->findLastMessage(prefix + "async call " + request_.client_id());

        // Spawn a new CallData instance to serve new clients while we process
        // the one for this CallData. The instance will deallocate itself as
        // part of its FINISH state.
        new CallData(service_, cq_, channel, cqClient);
    } else {
        GPR_ASSERT(status_ == FINISH);
        // Once in the FINISH state, deallocate ourselves (CallData).
        delete this;
    }
}

void CallData::Finish(const std::string &response) {
    reply_.set_message(response);

    // And we are done! Let the gRPC runtime know we've finished, using the
    // memory address of this instance as the uniquely identifying tag for
    // the event.
    status_ = FINISH;
    // Thread safe
    responder_.Finish(reply_, Status::OK, this);
}

void CallData::FinishWithError() {
    // And we are done! Let the gRPC runtime know we've finished, using the
    // memory address of this instance as the uniquely identifying tag for
    // the event.
    status_ = FINISH;
    // Thread safe
    responder_.FinishWithError(Status::CANCELLED, this);
}



class ServerImpl final {
public:
    ~ServerImpl() {
        server_->Shutdown();
        // Always shutdown the completion queue after the server.
        cq_->Shutdown();
        cq2_->Shutdown();
    }

    // There is no shutdown handling in this code.
    void Run() {
        std::string server_address("0.0.0.0:50052");

        ServerBuilder builder;
        // Listen on the given address without any authentication mechanism.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Register "service_" as the instance through which we'll communicate with
        // clients. In this case it corresponds to an *asynchronous* service.
        builder.RegisterService(&service_);
        // Get hold of the completion queue used for the asynchronous communication
        // with the gRPC runtime.
        cq_ = builder.AddCompletionQueue();
        cq2_ = builder.AddCompletionQueue();
        // Finally assemble the server.
        server_ = builder.BuildAndStart();
        std::cout << "Server listening on " << server_address << std::endl;

        // Proceed to the server's main loop.
        std::thread thread1 = std::thread(&ServerImpl::HandleRpcs, this, cq_.get());
        std::thread thread2 = std::thread(&ServerImpl::HandleRpcs, this, cq2_.get());
        thread1.join();  // blocks forever
        thread2.join();  // blocks forever
    }

private:

    // This can be run in multiple threads if needed.
    void HandleRpcs(ServerCompletionQueue *cq) {
        std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051",
                                                                     grpc::InsecureChannelCredentials());

        auto cqClient = new grpc::CompletionQueue();
        std::thread threadClient = std::thread(&ServerImpl::HandleChannel, cqClient);

        // Spawn a new CallData instance to serve new clients.
        new CallData(&service_, cq, channel, cqClient);
        void *tag;  // uniquely identifies a request.
        bool ok;
        while (true) {
            // Block waiting to read the next event from the completion queue. The
            // event is uniquely identified by its tag, which in this case is the
            // memory address of a CallData instance.
            // The return value of Next should always be checked. This return value
            // tells us whether there is any kind of event or cq_ is shutting down.
            GPR_ASSERT(cq->Next(&tag, &ok));
            GPR_ASSERT(ok);
            static_cast<CallData *>(tag)->Proceed();
        }
    }

    static void HandleChannel(grpc::CompletionQueue *cq) {
        void *tag;  // uniquely identifies a request.
        bool ok;
        while (true) {
            // Block waiting to read the next event from the completion queue. The
            // event is uniquely identified by its tag, which in this case is the
            // memory address of a CallData instance.
            // The return value of Next should always be checked. This return value
            // tells us whether there is any kind of event or cq_ is shutting down.
            GPR_ASSERT(cq->Next(&tag, &ok));
            GPR_ASSERT(ok);
            static_cast<M_asyncClient *>(tag)->handleCallback();
        }
    }

    std::unique_ptr<ServerCompletionQueue> cq_;
    std::unique_ptr<ServerCompletionQueue> cq2_;
    messageService::AsyncService service_;
    std::unique_ptr<Server> server_;
};


int main(int argc, char **argv) {
    ServerImpl server;
    server.Run();

    return 0;
}
