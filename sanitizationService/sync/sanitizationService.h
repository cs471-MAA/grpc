
#include "mock_message_board.grpc.pb.h"
#include "../../shared/Utils.h"

// Logic and data behind the server's behavior.
class sanitizationServiceImpl final : public mmb::sanitizationService::Service {
public:
    sanitizationServiceImpl();
    ::grpc::Status sanitize_message(::grpc::ServerContext* context, const ::mmb::saveMessageRequest* request, ::mmb::saveMessageReply* response) override;

private:
    std::unique_ptr<mmb::mockDatabase::Stub> mockDatabaseStub_;
};