//
// Created by adrien on 18.12.21.
//

#include "mockDatabaseAsync.h"
#include "asyncHandler.h"
#include "asyncSaveMessageHandler.h"
#include "asyncFindLastMessageHandler.h"
using grpc::ServerBuilder;


ServerAsyncImpl::ServerAsyncImpl(std::uint_fast32_t workerThreads, std::chrono::milliseconds waiting_time)
:threadPool(workerThreads), waiting_time(waiting_time){}

ServerAsyncImpl::~ServerAsyncImpl() {
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
}

void ServerAsyncImpl::Run() {
    std::string server_address("0.0.0.0:50051");

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service_" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service_);
    // Get hold of the completion queue used for the asynchronous communication
    // with the gRPC runtime.
    cq_ = builder.AddCompletionQueue();
    // Finally assemble the server.
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    // Proceed to the server's main loop.
    HandleRpcs();
}

void ServerAsyncImpl::HandleRpcs() {
    // Spawn a new CallData instance to serve new clients.
    new asyncFindLastMessageHandler(&service_, cq_.get(), threadPool, waiting_time, hashMap);
    new asyncSaveMessageHandler(&service_, cq_.get(), threadPool, waiting_time, hashMap);
    void* tag;  // uniquely identifies a request.
    bool ok;
    while (true) {
        // Block waiting to read the next event from the completion queue. The
        // event is uniquely identified by its tag, which in this case is the
        // memory address of a CallData instance.
        // The return value of Next should always be checked. This return value
        // tells us whether there is any kind of event or cq_ is shutting down.
        GPR_ASSERT(cq_->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<asyncHandler*>(tag)->Proceed(ok);
    }
}


int main(int argc, char** argv) {
    unsigned long workerThreads = 10;
    std::chrono::milliseconds waitingTime(100);

    ServerAsyncImpl server(workerThreads, waitingTime);
    server.Run();

    return 0;
}