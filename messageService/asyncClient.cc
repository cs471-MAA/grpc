//
// Created by adrien on 18.12.21.
//

#include "asyncClient.h"
#include "messageService.h"

M_asyncClient::M_asyncClient(const std::shared_ptr<grpc::Channel>& channel, grpc::CompletionQueue *cq,
                             CallData *callData)
                             : stub_(mockDatabase::NewStub(channel)), cq_(cq), callData_(callData) {}

// Assembles the client's payload, sends it and presents the response back
// from the server.
void M_asyncClient::findLastMessage(const std::string &cliend_id) {
    reply = findLastMessageReply();

    // Data we are sending to the server.
    request.set_client_id(cliend_id);

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

void M_asyncClient::handleCallback() {
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

