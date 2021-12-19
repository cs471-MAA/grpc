#include <thread>
#include "sanitizationServiceAsync.h"
#include "asyncSanitizeMessageHandler.h"
#include "../shared/consts.h"

sanitizationServiceAsyncImpl::sanitizationServiceAsyncImpl(std::uint_fast32_t workerThreads, std::chrono::microseconds waiting_time)
: threadPool(workerThreads), waiting_time(waiting_time){
    serverStats = std::make_shared<ServerStats2>(STATS_FILES_DIR "sanitService.csv");
}

sanitizationServiceAsyncImpl::~sanitizationServiceAsyncImpl() {
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
}

void sanitizationServiceAsyncImpl::Run() {
    std::string server_address = M_SANITIZATION_SERVICE_SOCKET_ADDRESS;

    grpc::ServerBuilder builder;
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
    std::thread thread1 = std::thread(&sanitizationServiceAsyncImpl::HandleRpcs, this, cq_.get());
    thread1.join();  // blocks forever
}

void sanitizationServiceAsyncImpl::HandleRpcs(ServerCompletionQueue *cq) {
    std::shared_ptr<grpc::Channel> DBchannel = grpc::CreateChannel(M_MOCK_DATABASE_SOCKET_ADDRESS,grpc::InsecureChannelCredentials());

    auto cqClient = new grpc::CompletionQueue();
    std::thread threadClient = std::thread(&sanitizationServiceAsyncImpl::HandleChannel, cqClient);

    // Spawn a new CallData instance to serve new clients.
    new asyncSanitizeMessageHandler(&service_, cq, DBchannel, cqClient, threadPool, waiting_time, serverStats);
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

void sanitizationServiceAsyncImpl::HandleChannel(CompletionQueue *cq) {
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
    unsigned long workerThreads = 10;
    std::chrono::microseconds waitingTime(100);

    sanitizationServiceAsyncImpl server(workerThreads, waitingTime);
    server.Run();

    return 0;
}


