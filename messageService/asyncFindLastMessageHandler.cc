#include <thread>
#include <sstream>
#include "asyncFindLastMessageHandler.h"
#include "../shared/Utils.h"

using grpc::Status;


findLastMessage_asyncClient::findLastMessage_asyncClient(
        const std::shared_ptr<grpc::ChannelInterface> &channel, grpc::CompletionQueue *cq,
        asyncFindLastMessageHandler *callData)
        : stub_(mmb::mockDatabase::NewStub(channel)), cq_(cq), callData_(callData) {}

// Assembles the client's payload, sends it and presents the response back
// from the server.
void findLastMessage_asyncClient::findLastMessage(findLastMessageRequest &request) {
    reply = findLastMessageReply();

    // You can declare error and stuff
    status = Status();

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    rpc = (stub_->PrepareAsyncfindLastMessage(&context, request, cq_));

    // StartCall initiates the RPC call
    rpc->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful.
    rpc->Finish(&reply, &status, this);
}

void findLastMessage_asyncClient::Proceed(bool ok) {
    // Act upon the status of the actual RPC.
    if (status.ok()) {
        //std::cout << bidule << std::endl;
        callData_->Finish(reply);
    } else {
        //std::cout << "ass RPC failed" << std::endl;
        callData_->FinishWithError();
    }

    delete this;
}

// ###############################################

asyncFindLastMessageHandler::asyncFindLastMessageHandler(messageService::AsyncService *service,
                                                         ServerCompletionQueue *cq,
                                                         std::shared_ptr<grpc::ChannelInterface> channel,
                                                         grpc::CompletionQueue *cqClient,
                                                         std::shared_ptr<ServerStats2> serverStats)
        : service_(service), cq_(cq), responder_(&ctx_), status_(PROCESS), cqClient(cqClient),
          channel(std::move(channel)),
          serverStats(std::move(serverStats)) {

    // As part of the initial CREATE state, we *request* that the system
    // start processing SayHello requests. In this request, "this" acts are
    // the tag uniquely identifying the request (so that different CallData
    // instances can serve different requests concurrently), in this case
    // the memory address of this CallData instance.
    service_->RequestfindLastMessage(&ctx_, &request_, &responder_, cq_, cq_, this);
}

void asyncFindLastMessageHandler::Proceed(bool ok) {
    if (status_ == PROCESS) {
        serverStats->add_entry(request_.query_uid(), get_epoch_time_us());

        auto asyncClient = new findLastMessage_asyncClient(channel, cqClient, this);
        asyncClient->findLastMessage(request_);

        // Spawn a new CallData instance to serve new clients while we process
        // the one for this CallData. The instance will deallocate itself as
        // part of its FINISH state.
        new asyncFindLastMessageHandler(service_, cq_, channel, cqClient, serverStats);
    } else {
        GPR_ASSERT(status_ == FINISH);
        serverStats->add_entry(request_.query_uid(), get_epoch_time_us());
        // Once in the FINISH state, deallocate ourselves (CallData).
        delete this;
    }
}

void asyncFindLastMessageHandler::Finish(findLastMessageReply &reply) {
    // And we are done! Let the gRPC runtime know we've finished, using the
    // memory address of this instance as the uniquely identifying tag for
    // the event.
    status_ = FINISH;
    // Thread safe
    responder_.Finish(reply, Status::OK, this);
}

void asyncFindLastMessageHandler::FinishWithError() {
    // And we are done! Let the gRPC runtime know we've finished, using the
    // memory address of this instance as the uniquely identifying tag for
    // the event.
    status_ = FINISH;
    // Thread safe
    responder_.FinishWithError(Status::CANCELLED, this);
}