//
// Created by adrien on 18.12.21.
//

#include "mockDatabaseSync.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>

using grpc::ServerBuilder;
using grpc::Server;


mockDatabaseSyncImpl::mockDatabaseSyncImpl() {

}

mockDatabaseSyncImpl::~mockDatabaseSyncImpl() {

}

Status
mockDatabaseSyncImpl::findLastMessage(::grpc::ServerContext *context, const ::mmb::findLastMessageRequest *request,
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

int main(int argc, char **argv) {
    RunSyncServer();

    return 0;
}