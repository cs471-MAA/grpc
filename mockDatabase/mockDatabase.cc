
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

mockDatabaseSyncImpl::mockDatabaseSyncImpl() {

}

mockDatabaseSyncImpl::~mockDatabaseSyncImpl() {

}

Status mockDatabaseSyncImpl::findLastMessage(::grpc::ServerContext *context, const ::mmb::findLastMessageRequest *request,
                                             ::mmb::findLastMessageReply *response) {
    response->set_query_uid(request->query_uid());
    response->set_message("La la la la");

    return Service::findLastMessage(context, request, response);
}

Status mockDatabaseSyncImpl::saveMessage(::grpc::ServerContext *context, const ::mmb::saveMessageRequest *request,
                                         ::mmb::saveMessageReply *response) {
    response->set_query_uid(request->query_uid());
    response->set_ok(true);
    return Service::saveMessage(context, request, response);
}

void RunSyncServer() {
    std::string server_address("0.0.0.0:50051");
    mockDatabaseSyncImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}


class ServerAsyncImpl final {
public:
    ~ServerAsyncImpl() {
        server_->Shutdown();
        // Always shutdown the completion queue after the server.
        cq_->Shutdown();
    }

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
    // Class encompasing the state and logic needed to serve a request.
    class CallData {
    public:
        // Take in the "service" instance (in this case representing an asynchronous
        // server) and the completion queue "cq" used for asynchronous communication
        // with the gRPC runtime.
        CallData(mockDatabase::AsyncService* service, ServerCompletionQueue* cq)
                : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {

            // As part of the initial CREATE state, we *request* that the system
            // start processing SayHello requests. In this request, "this" acts are
            // the tag uniquely identifying the request (so that different CallData
            // instances can serve different requests concurrently), in this case
            // the memory address of this CallData instance.
            status_ = PROCESS;
            service_->RequestfindLastMessage(&ctx_, &request_, &responder_, cq_, cq_,
                                      this);
        }

        void Proceed() {
            if (status_ == PROCESS) {

                // The actual processing.
                std::string prefix("Hello from server2 ");
                reply_.set_message(prefix + request_.client_id());

                // And we are done! Let the gRPC runtime know we've finished, using the
                // memory address of this instance as the uniquely identifying tag for
                // the event.
                status_ = FINISH;
                responder_.Finish(reply_, Status::OK, this);
                // Spawn a new CallData instance to serve new clients while we process
                // the one for this CallData. The instance will deallocate itself as
                // part of its FINISH state.
                new CallData(service_, cq_);
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
        enum CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_;  // The current serving state.
    };

    // This can be run in multiple threads if needed.
    void HandleRpcs() {
        // Spawn a new CallData instance to serve new clients.
        new CallData(&service_, cq_.get());
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
            static_cast<CallData*>(tag)->Proceed();
        }
    }

    std::unique_ptr<ServerCompletionQueue> cq_;
    mockDatabase::AsyncService service_;
    std::unique_ptr<Server> server_;
};



int main(int argc, char** argv) {
    ServerAsyncImpl server;
    server.Run();

    return 0;
}