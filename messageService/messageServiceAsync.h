#include <grpcpp/grpcpp.h>
#include "mock_message_board.grpc.pb.h"
#include "../shared/ServerStats2.h"

using grpc::ServerCompletionQueue;
using grpc::CompletionQueue;
using grpc::Server;
using mmb::mockDatabase;

class messageServiceAsyncImpl final {
public:
    messageServiceAsyncImpl();
    ~messageServiceAsyncImpl();

    // There is no shutdown handling in this code.
    void Run();

private:

    // This can be run in multiple threads if needed.
    void HandleRpcs(ServerCompletionQueue *cq);

    static void HandleChannel(CompletionQueue *cq);

    std::unique_ptr<ServerCompletionQueue> cq_;
    std::unique_ptr<ServerCompletionQueue> cq2_;
    mmb::messageService::AsyncService service_;
    std::unique_ptr<Server> server_;
    std::shared_ptr<ServerStats2> serverStats;
};