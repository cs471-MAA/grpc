#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "mock_message_board.grpc.pb.h"
#include "../shared/thread_pool.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using mmb::mockDatabase;
using mmb::findLastMessageRequest;
using mmb::findLastMessageReply;
using mmb::saveMessageRequest;
using mmb::saveMessageReply;
