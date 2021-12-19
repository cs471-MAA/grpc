#include <grpcpp/grpcpp.h>
#include "mock_message_board.grpc.pb.h"
#include "../shared/thread_pool.h"
#include "../shared/ServerStats2.h"

using grpc::ServerCompletionQueue;
using grpc::CompletionQueue;
using grpc::Server;
using mmb::mockDatabase;

class sanitizationServiceAsyncImpl final {
public:
    sanitizationServiceAsyncImpl(std::uint_fast32_t workerThreads, std::chrono::microseconds waiting_time);
    ~sanitizationServiceAsyncImpl();

    // There is no shutdown handling in this code.
    void Run();

private:

    // This can be run in multiple threads if needed.
    void HandleRpcs(ServerCompletionQueue *cq);

    static void HandleChannel(CompletionQueue *cq);

    std::unique_ptr<ServerCompletionQueue> cq_;
    mmb::sanitizationService::AsyncService service_;
    std::unique_ptr<Server> server_;
    thread_pool threadPool;
    std::chrono::microseconds waiting_time;
    std::shared_ptr<ServerStats2> serverStats;
};