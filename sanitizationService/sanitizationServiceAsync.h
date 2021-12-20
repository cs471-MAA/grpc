#include <grpcpp/grpcpp.h>
#include "mock_message_board.grpc.pb.h"
#include "../shared/thread_pool.h"
#include "../shared/ServerStats2.h"
#include "../shared/Utils.h"

using grpc::ServerCompletionQueue;
using grpc::CompletionQueue;
using grpc::Server;
using mmb::mockDatabase;

class sanitizationServiceAsyncImpl final {
public:
    sanitizationServiceAsyncImpl(uint32_t meanWaitingTime, uint32_t stdWaitingTime);

    ~sanitizationServiceAsyncImpl();

    // There is no shutdown handling in this code.
    void Run(unsigned long workerThreads);

private:

    // This can be run in multiple threads if needed.
    void HandleRpcs(ServerCompletionQueue *cq);

    static void HandleChannel(CompletionQueue *cq);

    std::vector<std::unique_ptr<ServerCompletionQueue>> cqVect;
    std::vector<std::thread> workerThreadsVect;
    mmb::sanitizationService::AsyncService service_;
    std::unique_ptr<Server> server_;
    uint32_t meanWaitingTime;
    uint32_t stdWaitingTime;
    std::shared_ptr<ServerStats2> serverStats;
};