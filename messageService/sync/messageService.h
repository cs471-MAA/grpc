
#include "mock_message_board.grpc.pb.h"
#include "../../shared/Utils.h"
#include "../../shared/ServerStats2.h"
// Logic and data behind the server's behavior.
class messageServiceImpl final : public mmb::messageService::Service {
public:
    messageServiceImpl(uint32_t meanWaitingTime, uint32_t stdWaitingTime);
    ::grpc::Status findLastMessage(::grpc::ServerContext* context, const ::mmb::findLastMessageRequest* request, ::mmb::findLastMessageReply* response) override;
    ::grpc::Status sendMessage(::grpc::ServerContext* context, const ::mmb::saveMessageRequest* request, ::mmb::saveMessageReply* response) override;

private:
    uint32_t meanWaitingTime;
    uint32_t stdWaitingTime;
    std::unique_ptr<mmb::mockDatabase::Stub> mockDatabaseStub_;
    std::unique_ptr<mmb::sanitizationService::Stub> sanitizationServiceStub_;
    std::shared_ptr<ServerStats2> serverStats;
};