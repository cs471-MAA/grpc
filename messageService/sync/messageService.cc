//
// Created by adrien on 19.12.21.
//

#include "messageService.h"
#include "../../shared/consts.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/resource_quota.h>
#include <thread>

using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;
using namespace std;

messageServiceImpl::messageServiceImpl(uint32_t meanWaitingTime,
                                       uint32_t stdWaitingTime):
    meanWaitingTime(meanWaitingTime), 
    stdWaitingTime(stdWaitingTime),
    serverStats(std::make_shared<ServerStats2>(STATS_FILES_DIR MESSAGE_SERVICE_SYNC_FILENAME))
{
    mockDatabaseStub_ = mmb::mockDatabase::NewStub(grpc::CreateChannel(M_MOCK_DATABASE_SYNC_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()));
    sanitizationServiceStub_ = mmb::sanitizationService::NewStub(grpc::CreateChannel(M_SANITIZATION_SERVICE_SYNC_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()));

}

::grpc::Status
messageServiceImpl::findLastMessage(::grpc::ServerContext *context, const ::mmb::findLastMessageRequest *request,
                                    ::mmb::findLastMessageReply *response) {

    serverStats->add_entry(request->query_uid(), get_epoch_time_us());
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.

    grpc::ClientContext clientContext_1;
    findLastMessageReply response2_1;
    auto result = mockDatabaseStub_->findLastMessage(&clientContext_1, *request, &response2_1);

    grpc::ClientContext clientContext_2;
    findLastMessageReply response2_2;
    auto result2 = mockDatabaseStub_->findLastMessage(&clientContext_2, *request, &response2_2);

    grpc::ClientContext clientContext_3;
    findLastMessageReply response2_3;
    auto result3 = mockDatabaseStub_->findLastMessage(&clientContext_3, *request, &response2_3);

    response->set_message(response2_1.message() + response2_2.message() + response2_3.message());
    response->set_query_uid(response2_1.query_uid());
    auto work = fake_worker(meanWaitingTime);
    response->set_compute(work);

    serverStats->add_entry(request->query_uid(), get_epoch_time_us());

    return result;
}

::grpc::Status messageServiceImpl::sendMessage(::grpc::ServerContext *context, const ::mmb::saveMessageRequest *request,
                                               ::mmb::saveMessageReply *response) {

    serverStats->add_entry(request->query_uid(), get_epoch_time_us());
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext clientContext_1;
    saveMessageReply response2_1;
    auto result = sanitizationServiceStub_->sanitize_message(&clientContext_1, *request, &response2_1);

    grpc::ClientContext clientContext_2;
    saveMessageReply response2_2;
    auto result2 = sanitizationServiceStub_->sanitize_message(&clientContext_2, *request, &response2_2);

    grpc::ClientContext clientContext_3;
    saveMessageReply response2_3;
    auto result3 = sanitizationServiceStub_->sanitize_message(&clientContext_3, *request, &response2_3);

    response->set_query_uid(response2_1.query_uid());
    response->set_ok(response2_1.ok() & response2_2.ok() & response2_3.ok());
    auto work = fake_worker(meanWaitingTime);
    response->set_compute(work);
    serverStats->add_entry(request->query_uid(), get_epoch_time_us());

    return result;
}


void RunServer(int workerThreads,
               uint32_t meanWaitingTime,
               uint32_t stdWaitingTime) {
    string server_address(M_MESSAGE_SERVICE_SYNC_SOCKET_ADDRESS);
    messageServiceImpl service(meanWaitingTime, stdWaitingTime);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);

    // auto r = grpc::ResourceQuota();
    // builder.SetResourceQuota(r.SetMaxThreads(workerThreads + 1));
    // builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::NUM_CQS, workerThreads);
    // builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MAX_POLLERS , workerThreads);
    // builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::CQ_TIMEOUT_MSEC, 100000);
    // Finally assemble the server

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    int i = 0;
    int workerThreads = (argc > ++i) ? stoi(argv[i]) : 5;
    uint32_t meanWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 1000);
    uint32_t stdWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 500);

    RunServer(workerThreads, meanWaitingTime, stdWaitingTime);

    return 0;
}

