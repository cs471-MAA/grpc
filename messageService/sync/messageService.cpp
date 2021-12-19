//
// Created by adrien on 19.12.21.
//

#include "messageService.h"
#include "../shared/consts.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;


messageServiceImpl::messageServiceImpl() {
    mockDatabaseStub_ = mmb::mockDatabase::NewStub(grpc::CreateChannel(MOCK_DATABASE_SYNC_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()));
    sanitizationServiceStub_ = mmb::sanitizationService::NewStub(grpc::CreateChannel(M_SANITIZATION_SERVICE_SYNC_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()));
}

::grpc::Status
messageServiceImpl::findLastMessage(::grpc::ServerContext *context, const ::mmb::findLastMessageRequest *request,
                                    ::mmb::findLastMessageReply *response) {

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext clientContext;

    return mockDatabaseStub_->findLastMessage(&clientContext, *request, response);
}

::grpc::Status messageServiceImpl::sendMessage(::grpc::ServerContext *context, const ::mmb::saveMessageRequest *request,
                                               ::mmb::saveMessageReply *response) {

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext clientContext;

    return sanitizationServiceStub_->sanitize_message(&clientContext, *request, response);
}




void RunServer() {
    std::string server_address(M_MESSAGE_SERVICE_SYNC_SOCKET_ADDRESS);
    messageServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();

    return 0;
}

