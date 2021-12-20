//
// Created by adrien on 18.12.21.
//

#include "mockDatabase.h"
#include "../../shared/consts.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/resource_quota.h>
#include <thread>
#include <utility>

using grpc::ResourceQuota;
using grpc::ServerBuilder;
using grpc::Server;
using namespace std;

mockDatabaseImpl::mockDatabaseImpl(uint32_t meanWaitingTime,
                                  uint32_t stdWaitingTime, std::shared_ptr<CTSL::HashMap<std::string, std::string>> hashMap):
    meanWaitingTime(meanWaitingTime), 
    stdWaitingTime(stdWaitingTime),
    serverStats(std::make_shared<ServerStats2>(STATS_FILES_DIR MOCK_DATABASE_SYNC_FILENAME)),
    hashMap(std::move(hashMap)){}

Status
mockDatabaseImpl::findLastMessage(::grpc::ServerContext *context, const ::mmb::findLastMessageRequest *request,
                                      ::mmb::findLastMessageReply *response) {
    serverStats->add_entry(request->query_uid(), get_epoch_time_us());
    response->set_query_uid(request->query_uid());

    std::string result;
    if (hashMap->find(request->client_id(), result)) {
        response->set_message(result);
    } else {
        response->set_message("Client ID not found");
    }

    auto work = fake_worker(meanWaitingTime);
    response->set_compute(work);
    serverStats->add_entry(request->query_uid(), get_epoch_time_us());
    return {};
}

Status mockDatabaseImpl::saveMessage(::grpc::ServerContext *context, const ::mmb::saveMessageRequest *request,
                                         ::mmb::saveMessageReply *response) {
    serverStats->add_entry(request->query_uid(), get_epoch_time_us());
    response->set_query_uid(request->query_uid());
    response->set_ok(true);
    hashMap->insert(request->client_id(), request->message());


    auto work = fake_worker(meanWaitingTime);
    response->set_compute(work);
    serverStats->add_entry(request->query_uid(), get_epoch_time_us());
    return {};
}

void RunServer(int workerThreads,
                   uint32_t meanWaitingTime,
                   uint32_t stdWaitingTime) {
    std::string server_address(M_MOCK_DATABASE_SYNC_SOCKET_ADDRESS);
    auto hashMap = std::make_shared<CTSL::HashMap<std::string, std::string>>();
    mockDatabaseImpl service(meanWaitingTime, stdWaitingTime, hashMap);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
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

    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char **argv) {
    int i = 0;
    int workerThreads = (argc > ++i) ? stoi(argv[i]) : 5;
    uint32_t meanWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 3000);
    uint32_t stdWaitingTime = ((argc > ++i) ? stoi(argv[i]) : 1000);

    RunServer(workerThreads, meanWaitingTime, stdWaitingTime);
    return 0;
}