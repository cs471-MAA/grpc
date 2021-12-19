
#include "mock_message_board.grpc.pb.h"
#include <grpcpp/grpcpp.h>

using grpc::Status;
using mmb::mockDatabase;

// Logic and data behind the server's behavior.
class mockDatabaseImpl final : public mockDatabase::Service {
public:
    mockDatabaseImpl() = default;

    ~mockDatabaseImpl() override = default;

    Status findLastMessage(::grpc::ServerContext *context, const ::mmb::findLastMessageRequest *request,
                           ::mmb::findLastMessageReply *response) override;

    Status saveMessage(::grpc::ServerContext *context, const ::mmb::saveMessageRequest *request,
                       ::mmb::saveMessageReply *response) override;
};
