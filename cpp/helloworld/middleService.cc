/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include <sstream>
#include <utility>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else

#include "helloworld.grpc.pb.h"

#endif

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;


// Class encompasing the state and logic needed to serve a request.
class CallData {
public:
    // Take in the "service" instance (in this case representing an asynchronous
    // server) and the completion queue "cq" used for asynchronous communication
    // with the gRPC runtime.
    CallData(Greeter::AsyncService *service, ServerCompletionQueue *cq, const std::shared_ptr<grpc::Channel> &channel,
             grpc::CompletionQueue *cqClient)
            : service_(service), cq_(cq), responder_(&ctx_), status_(PROCESS), cqClient(cqClient), channel(channel) {


        // As part of the initial CREATE state, we *request* that the system
        // start processing SayHello requests. In this request, "this" acts are
        // the tag uniquely identifying the request (so that different CallData
        // instances can serve different requests concurrently), in this case
        // the memory address of this CallData instance.
        service_->RequestSayHello(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
    }

    void Proceed() {
        if (status_ == PROCESS) {
            std::stringstream ss;
            ss << std::this_thread::get_id();

            // The actual processing.
            std::string prefix("Hello from server1 threadID " + ss.str());
            //std::string bidule = greeter.SayHello("server1");

            auto asyncClient = new AsyncGreeterClient(channel, cqClient, this);
            asyncClient->SayHello(prefix + "async call " + request_.name());

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

    // It's ugly, it should be in a separate file but this is an easy way to avoid cyclic dependency hell
    class AsyncGreeterClient {
    public:
        explicit AsyncGreeterClient(const std::shared_ptr<grpc::Channel> &channel, grpc::CompletionQueue *cq,
                                    CallData *callData)
                : stub_(Greeter::NewStub(channel)), cq_(cq), callData_(callData) {}

        // Assembles the client's payload, sends it and presents the response back
        // from the server.
        void SayHello(const std::string &user) {
            // Data we are sending to the server.
            reply = HelloReply();

            request.set_name(user);

            // You can declare error and stuff
            status = Status();


            // stub_->PrepareAsyncSayHello() creates an RPC object, returning
            // an instance to store in "call" but does not actually start the RPC
            // Because we are using the asynchronous API, we need to hold on to
            // the "call" instance in order to get updates on the ongoing RPC.
            rpc = (stub_->PrepareAsyncSayHello(&context, request, cq_));

            // StartCall initiates the RPC call
            rpc->StartCall();

            // Request that, upon completion of the RPC, "reply" be updated with the
            // server's response; "status" with the indication of whether the operation
            // was successful.
            rpc->Finish(&reply, &status, this);
        }

        // Will be called by the HandleChannel thread because of the cq_ variable this class use is the one that's looped on
        // in the HandleChannel function
        void handleCallback() {
            // Act upon the status of the actual RPC.
            if (status.ok()) {
                auto bidule = reply.message();
                //std::cout << bidule << std::endl;
                callData_->Finish(bidule);
            } else {
                //std::cout << "ass RPC failed" << std::endl;
                callData_->FinishWithError();
            }

            delete this;
        }

    private:
        // Everything has to be declared as class variable because of pointer magic that will make code from the HandleChannel
        // thread write stuff in this class (with handleCallback()) that was created by the HandleRPC thread

        // Out of the passed in Channel comes the stub, stored here, our view of the
        // server's exposed services.
        std::unique_ptr<Greeter::Stub> stub_;
        grpc::CompletionQueue *cq_{};

        // Container for the data we expect from the server.
        HelloReply reply;
        // Storage for the status of the RPC upon completion.
        Status status;
        std::unique_ptr<grpc::ClientAsyncResponseReader<HelloReply>> rpc;
        HelloRequest request;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        grpc::ClientContext context;

        // The means to get back to the client.
        CallData *callData_;
    };

private:
    void Finish(const std::string &response) {
        reply_.set_message(response);

        // And we are done! Let the gRPC runtime know we've finished, using the
        // memory address of this instance as the uniquely identifying tag for
        // the event.
        status_ = FINISH;
        // Thread safe
        responder_.Finish(reply_, Status::OK, this);
    }

    void FinishWithError() {
        // And we are done! Let the gRPC runtime know we've finished, using the
        // memory address of this instance as the uniquely identifying tag for
        // the event.
        status_ = FINISH;
        // Thread safe
        responder_.FinishWithError(Status::CANCELLED, this);
    }

    grpc::CompletionQueue *cqClient;

    const std::shared_ptr<grpc::Channel> &channel;
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    Greeter::AsyncService *service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue *cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    HelloRequest request_;
    // What we send back to the client.
    HelloReply reply_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<HelloReply> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus {
        PROCESS, FINISH
    };
    CallStatus status_;  // The current serving state.
};


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
        std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50052",
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
            static_cast<CallData::AsyncGreeterClient *>(tag)->handleCallback();
        }
    }

    std::unique_ptr<ServerCompletionQueue> cq_;
    std::unique_ptr<ServerCompletionQueue> cq2_;
    Greeter::AsyncService service_;
    std::unique_ptr<Server> server_;
};


int main(int argc, char **argv) {
    ServerImpl server;
    server.Run();

    return 0;
}
