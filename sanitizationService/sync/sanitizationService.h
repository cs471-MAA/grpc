
#include "mock_message_board.grpc.pb.h"
#include "../../shared/Utils.h"
#include "../../shared/ServerStats2.h"

// Logic and data behind the server's behavior.
class sanitizationServiceImpl final : public mmb::sanitizationService::Service {
public:
    sanitizationServiceImpl(uint32_t meanWaitingTime,
                            uint32_t stdWaitingTime);
    ::grpc::Status sanitize_message(::grpc::ServerContext* context, const ::mmb::saveMessageRequest* request, ::mmb::saveMessageReply* response) override;

private:
    std::unique_ptr<mmb::mockDatabase::Stub> mockDatabaseStub_;
  
    uint32_t meanWaitingTime;
    uint32_t stdWaitingTime;
    std::shared_ptr<ServerStats2> serverStats;
};