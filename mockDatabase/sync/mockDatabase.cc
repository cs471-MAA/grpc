//
// Created by adrien on 18.12.21.
//

#include "mockDatabase.h"
#include "../../shared/consts.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/resource_quota.h>
#include <thread>

using grpc::ResourceQuota;
using grpc::ServerBuilder;
using grpc::Server;
using namespace std;

mockDatabaseImpl::mockDatabaseImpl(uint32_t meanWaitingTime,
                                  uint32_t stdWaitingTime):
    meanWaitingTime(meanWaitingTime), 
    stdWaitingTime(stdWaitingTime) {}

Status
mockDatabaseImpl::findLastMessage(::grpc::ServerContext *context, const ::mmb::findLastMessageRequest *request,
                                      ::mmb::findLastMessageReply *response) {
    response->set_query_uid(request->query_uid());
    response->set_message("La la la la");
    
    this_thread::sleep_for(normal_distributed_value(meanWaitingTime, stdWaitingTime) * 1us);

    return Service::findLastMessage(context, request, response);
}

Status mockDatabaseImpl::saveMessage(::grpc::ServerContext *context, const ::mmb::saveMessageRequest *request,
                                         ::mmb::saveMessageReply *response) {
    response->set_query_uid(request->query_uid());
    response->set_ok(true);

    this_thread::sleep_for(normal_distributed_value(meanWaitingTime, stdWaitingTime) * 1us);

    return Service::saveMessage(context, request, response);
}

void RunServer(unsigned long workerThreads,
                   uint32_t meanWaitingTime,
                   uint32_t stdWaitingTime) {
    std::string server_address(MOCK_DATABASE_SYNC_SOCKET_ADDRESS);
    mockDatabaseImpl service(meanWaitingTime, stdWaitingTime);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    auto r = grpc::ResourceQuota();
    builder.SetResourceQuota(r.SetMaxThreads(workerThreads));

    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char **argv) {
    int i = 0;
    unsigned long workerThreads = (argc > ++i) ? stoi(argv[i]) : 1;
    uint32_t meanWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 3000);
    uint32_t stdWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 1000);

    RunServer(workerThreads, meanWaitingTime, stdWaitingTime);
    return 0;
}