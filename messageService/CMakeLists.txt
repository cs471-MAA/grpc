
add_executable(messageServiceAsync messageService/asyncFindLastMessageHandler.h messageService/asyncSanitizeMessageHandler.h
        messageService/messageService.h messageService/asyncFindLastMessageHandler.cc
        messageService/asyncSanitizeMessageHandler.cc messageService/messageService.cc)
target_link_libraries(messageServiceAsync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})