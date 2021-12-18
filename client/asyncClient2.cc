#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "mock_message_board.grpc.pb.h"
#include "asyncClient2.h"

AsyncClient::AsyncClient(const std::shared_ptr<Channel> &channel)
        : stub_(messageService::NewStub(channel)){}

void AsyncClient::findLastMessage(const std::string &cliend_id) {
    // Data we are sending to the server.
    findLastMessageRequest request;
    request.set_client_id(cliend_id);

    // Call object to store rpc data
    auto* call = new AsC_findLastMessageCall;

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->response_reader =
            stub_->PrepareAsyncfindLastMessage(&call->context, request, &cq_);

    // StartCall initiates the RPC call
    call->response_reader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->response_reader->Finish(&call->reply, &call->status, (void*)call);
}

void AsyncClient::AsyncCompleteRpc() {
    void* got_tag;
    bool ok = false;

    // Block until the next result is available in the completion queue "cq".
    while (cq_.Next(&got_tag, &ok)) {
        // The tag in this example is the memory location of the call object
        auto* call = static_cast<AsC_findLastMessageCall*>(got_tag);

        // Verify that the request was completed successfully. Note that "ok"
        // corresponds solely to the request for updates introduced by Finish().
        GPR_ASSERT(ok);

        if (call->status.ok()){
            std::cout << "Greeter received: " << call->reply.message() << " " << std::this_thread::get_id() << std::endl;
        }
        else{
            std::cout << "RPC failed" << std::endl;
            std::cout << call->status.error_details() << std::endl;
        }

        // Once we're complete, deallocate the call object.
        delete call;
    }
}

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  AsyncClient client(grpc::CreateChannel(
      "localhost:50052", grpc::InsecureChannelCredentials()));

  // Spawn reader thread that loops indefinitely
  // only calls findmessage atm
  std::thread thread_ = std::thread(&AsyncClient::AsyncCompleteRpc, &client);
  std::thread thread2_ = std::thread(&AsyncClient::AsyncCompleteRpc, &client);

  for (int i = 0; i < 10000; i++) {
    std::string user("world " + std::to_string(i));
      client.findLastMessage(user);  // The actual RPC call!
  }

  std::cout << "Press control-c to quit" << std::endl << std::endl;
  thread_.join();  // blocks forever
  thread2_.join();  // blocks forever

  return 0;
}

