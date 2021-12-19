//
// Created by adrien on 19.12.21.
//

#include <thread>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "sanitizationService.h"
#include "../../shared/consts.h"

using mmb::saveMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;

sanitizationServiceImpl::sanitizationServiceImpl(uint32_t meanWaitingTime,
                                                 uint32_t stdWaitingTime):
    meanWaitingTime(meanWaitingTime), 
    stdWaitingTime(stdWaitingTime)  
{
    mockDatabaseStub_ = mmb::mockDatabase::NewStub(grpc::CreateChannel(MOCK_DATABASE_SYNC_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()));
}

::grpc::Status
sanitizationServiceImpl::sanitize_message(::grpc::ServerContext *context, const ::mmb::saveMessageRequest *request,
                                          ::mmb::saveMessageReply *response) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext clientContext;

    auto result = mockDatabaseStub_->saveMessage(&clientContext, *request, response);
    this_thread::sleep_for(normal_distributed_value(meanWaitingTime, stdWaitingTime) * 1us);
    return result;
}


void RunServer(int workerThreads,
               uint32_t meanWaitingTime,
               uint32_t stdWaitingTime) {
    std::string server_address(M_SANITIZATION_SERVICE_SYNC_SOCKET_ADDRESS);
    sanitizationServiceImpl service(meanWaitingTime, stdWaitingTime);

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
    int i = 0;
    int workerThreads = (argc > ++i) ? stoi(argv[i]) : 5;
    uint32_t meanWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 3000);
    uint32_t stdWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 1000);
    RunServer(workerThreads, meanWaitingTime, stdWaitingTime);

    return 0;
}

