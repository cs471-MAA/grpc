
add_executable(messageServiceAsync shared/consts.h messageService/asyncFindLastMessageHandler.h messageService/asyncSendMessageHandler.h
        messageService/messageServiceAsync.h messageService/asyncFindLastMessageHandler.cc
        messageService/asyncSendMessageHandler.cc messageService/messageServiceAsync.cc)
target_link_libraries(messageServiceAsync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})