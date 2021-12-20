#include <grpcpp/grpcpp.h>
#include "mock_message_board.grpc.pb.h"
#include "../shared/ServerStats2.h"
#include "../shared/Utils.h"
#include "../shared/thread_pool.h"

using grpc::ServerCompletionQueue;
using grpc::CompletionQueue;
using grpc::Server;
using mmb::mockDatabase;

class messageServiceAsyncImpl final {
public:
    messageServiceAsyncImpl(uint32_t meanWaitingTime, uint32_t stdWaitingTime);

    ~messageServiceAsyncImpl();

    // There is no shutdown handling in this code.
    void Run(unsigned long workerThreads);

private:

    // This can be run in multiple threads if needed.
    void HandleRpcs(ServerCompletionQueue *cq);

    static void HandleChannel(CompletionQueue *cq);

    std::vector<std::unique_ptr<ServerCompletionQueue>> cqVect;
    std::vector<std::thread> workerThreadsVect;
    std::unique_ptr<ServerCompletionQueue> cq_;
    std::unique_ptr<ServerCompletionQueue> cq2_;
    uint32_t meanWaitingTime;
    uint32_t stdWaitingTime;
    mmb::messageService::AsyncService service_;
    std::unique_ptr<Server> server_;
    std::shared_ptr<ServerStats2> serverStats;
};