#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "mock_message_board.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using mmb::mockDatabase;
using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;

// Logic and data behind the server's behavior.
class mockDatabaseImpl final : public mockDatabase::Service {
public:
    mockDatabaseImpl();
    ~mockDatabaseImpl() override;
    Status findLastMessage(::grpc::ServerContext* context, const ::mmb::findLastMessageRequest* request, ::mmb::findLastMessageReply* response) override;
    Status saveMessage(::grpc::ServerContext* context, const ::mmb::saveMessageRequest* request, ::mmb::saveMessageReply* response) override;

};
