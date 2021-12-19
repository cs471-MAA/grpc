#include <grpcpp/grpcpp.h>
#include "mock_message_board.grpc.pb.h"
#include <random>
#include "../shared/thread_pool.h"
#include "../shared/HashMap.h"
#include "../shared/ServerStats2.h"
#include "../shared/Utils.h"

using grpc::ServerCompletionQueue;
using grpc::Server;
using mmb::mockDatabase;


class ServerAsyncImpl final {
public:

    explicit ServerAsyncImpl(std::uint_fast32_t workerThreads, uint32_t meanWaitingTime, uint32_t stdWaitingTime);

    ~ServerAsyncImpl();

    // There is no shutdown handling in this code.
    void Run();

private:
    // This can be run in multiple threads if needed.
    void HandleRpcs();

    std::unique_ptr<ServerCompletionQueue> cq_;
    mockDatabase::AsyncService service_;
    std::unique_ptr<Server> server_;
    thread_pool threadPool;
    uint32_t meanWaitingTime;
    uint32_t stdWaitingTime;
    CTSL::HashMap<std::string, std::string> hashMap;
    std::shared_ptr<ServerStats2> serverStats;
};