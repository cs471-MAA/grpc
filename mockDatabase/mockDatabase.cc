
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

mockDatabaseImpl::mockDatabaseImpl() {

}

mockDatabaseImpl::~mockDatabaseImpl() {

}

Status mockDatabaseImpl::findLastMessage(::grpc::ServerContext *context, const ::mmb::findLastMessageRequest *request,
                                         ::mmb::findLastMessageReply *response) {
    response->set_query_uid(request->query_uid());
    response->set_message("La la la la");

    return Service::findLastMessage(context, request, response);
}

Status mockDatabaseImpl::saveMessage(::grpc::ServerContext *context, const ::mmb::saveMessageRequest *request,
                                     ::mmb::saveMessageReply *response) {
    response->set_query_uid(request->query_uid());
    response->set_ok(true);
    return Service::saveMessage(context, request, response);
}

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    mockDatabaseImpl service;

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

int main(int argc, char** argv) {
    RunServer();

    return 0;
}