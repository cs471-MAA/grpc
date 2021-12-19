
#include "mock_message_board.grpc.pb.h"


// Logic and data behind the server's behavior.
class messageServiceImpl final : public mmb::messageService::Service {
public:
    messageServiceImpl();
    ::grpc::Status findLastMessage(::grpc::ServerContext* context, const ::mmb::findLastMessageRequest* request, ::mmb::findLastMessageReply* response) override;
    ::grpc::Status sendMessage(::grpc::ServerContext* context, const ::mmb::saveMessageRequest* request, ::mmb::saveMessageReply* response) override;

private:
    std::unique_ptr<mmb::mockDatabase::Stub> mockDatabaseStub_;
    std::unique_ptr<mmb::sanitizationService::Stub> sanitizationServiceStub_;
};