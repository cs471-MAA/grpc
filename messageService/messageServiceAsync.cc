#include <thread>
#include "messageServiceAsync.h"
#include "asyncFindLastMessageHandler.h"
#include "asyncSendMessageHandler.h"
#include "../shared/consts.h"


using namespace std;

messageServiceAsyncImpl::messageServiceAsyncImpl(uint32_t meanWaitingTime, uint32_t stdWaitingTime):
    meanWaitingTime(meanWaitingTime), stdWaitingTime(stdWaitingTime) {
    serverStats = std::make_shared<ServerStats2>(STATS_FILES_DIR MESSAGE_SERVICE_ASYNC_FILENAME);
}

messageServiceAsyncImpl::~messageServiceAsyncImpl() {
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
    cq2_->Shutdown();
}

void messageServiceAsyncImpl::Run(unsigned long workerThreads) {
    std::string server_address(M_MESSAGE_SERVICE_SOCKET_ADDRESS);

    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service_" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service_);
    // Get hold of the completion queue used for the asynchronous communication
    // with the gRPC runtime.
    for (unsigned long i = 0; i < workerThreads; ++i){
        cqVect.emplace_back(builder.AddCompletionQueue());
    }
    // Finally assemble the server.
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    // Proceed to the server's main loop.
    for (unsigned long i = 0; i < workerThreads; ++i){
        workerThreadsVect.emplace_back(std::thread(&messageServiceAsyncImpl::HandleRpcs, this, cqVect.at(i).get()));
    }

    workerThreadsVect.at(0).join();  // blocks forever
}

void messageServiceAsyncImpl::HandleRpcs(ServerCompletionQueue *cq) {
    std::shared_ptr<grpc::Channel> DBchannel = grpc::CreateChannel(M_MOCK_DATABASE_SOCKET_ADDRESS,
                                                                   grpc::InsecureChannelCredentials());
    std::shared_ptr<grpc::Channel> Sanitchannel = grpc::CreateChannel(M_SANITIZATION_SERVICE_SOCKET_ADDRESS,
                                                                      grpc::InsecureChannelCredentials());

    auto cqClient = new grpc::CompletionQueue();
    std::thread threadClient = std::thread(&messageServiceAsyncImpl::HandleChannel, cqClient);

    // Spawn a new CallData instance to serve new clients.
    new asyncFindLastMessageHandler(&service_, cq, DBchannel, cqClient, meanWaitingTime, stdWaitingTime, serverStats);
    new asyncSendMessageHandler(&service_, cq, Sanitchannel, cqClient, meanWaitingTime, stdWaitingTime, serverStats);
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
        // tells us whether there is any kind of evestoint or cq_ is shutting down.
        GPR_ASSERT(cq->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<asyncHandler *>(tag)->Proceed(ok);
    }
}


int main(int argc, char **argv) {
    cout << "Number of available cores: " << std::thread::hardware_concurrency() << endl;

    int i = 0;
    unsigned long workerThreads = (argc > ++i) ? stoi(argv[i]) : 1;
    uint32_t meanWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 20);
    uint32_t stdWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 20);

    messageServiceAsyncImpl server(meanWaitingTime, stdWaitingTime);
    server.Run(workerThreads);

    return 0;
}


