#include <thread>
#include "messageService.h"
#include "asyncFindLastMessageHandler.h"
#include "asyncSanitizeMessageHandler.h"

messageServiceAsyncImpl::~messageServiceAsyncImpl() {
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
    cq2_->Shutdown();
}

void messageServiceAsyncImpl::Run() {
    std::string server_address("0.0.0.0:50052");

    grpc::ServerBuilder builder;
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
    std::thread thread1 = std::thread(&messageServiceAsyncImpl::HandleRpcs, this, cq_.get());
    std::thread thread2 = std::thread(&messageServiceAsyncImpl::HandleRpcs, this, cq2_.get());
    thread1.join();  // blocks forever
    thread2.join();  // blocks forever
}

void messageServiceAsyncImpl::HandleRpcs(ServerCompletionQueue *cq) {
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051",
                                                                 grpc::InsecureChannelCredentials());

    auto cqClient = new grpc::CompletionQueue();
    std::thread threadClient = std::thread(&messageServiceAsyncImpl::HandleChannel, cqClient);

    // Spawn a new CallData instance to serve new clients.
    new asyncFindLastMessageHandler(&service_, cq, channel, cqClient);
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
        static_cast<asyncHandler *>(tag)->Proceed(ok);
    }
}

void messageServiceAsyncImpl::HandleChannel(CompletionQueue *cq) {
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
        static_cast<asyncHandler *>(tag)->Proceed(ok);
    }
}


int main(int argc, char **argv) {
    messageServiceAsyncImpl server;
    server.Run();

    return 0;
}


