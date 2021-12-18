
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include <grpcpp/health_check_service_interface.h>

#include "mock_message_board.grpc.pb.h"
#include "mockDatabase.h"
#include "asyncRequestHandler.h"
#include "../shared/thread_pool.h"




class ServerAsyncImpl final {
public:
    ~ServerAsyncImpl() {
        server_->Shutdown();
        // Always shutdown the completion queue after the server.
        cq_->Shutdown();
    }

    explicit ServerAsyncImpl(std::uint_fast32_t workerThreads, std::chrono::milliseconds waiting_time)
    : threadPool(workerThreads), waiting_time(waiting_time){}

    // There is no shutdown handling in this code.
    void Run() {
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

private:

    // This can be run in multiple threads if needed.
    void HandleRpcs() {
        // Spawn a new CallData instance to serve new clients.
        new asyncRequestHandler(&service_, cq_.get(), threadPool, waiting_time);
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
            static_cast<asyncRequestHandler*>(tag)->Proceed();
        }
    }

    std::unique_ptr<ServerCompletionQueue> cq_;
    mockDatabase::AsyncService service_;
    std::unique_ptr<Server> server_;
    thread_pool threadPool;
    std::chrono::milliseconds waiting_time;
};





