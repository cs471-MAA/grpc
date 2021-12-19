
add_executable(messageServiceAsync shared/consts.h messageService/asyncFindLastMessageHandler.h
        messageService/asyncSendMessageHandler.h shared/Utils.h shared/Utils.cc shared/ServerStats2.h shared/ServerStats2.cpp
        messageService/messageServiceAsync.h messageService/asyncFindLastMessageHandler.cc
        messageService/asyncSendMessageHandler.cc messageService/messageServiceAsync.cc)
target_link_libraries(messageServiceAsync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})


add_executable(messageService shared/consts.h shared/Utils.h shared/Utils.cc messageService/sync/messageService.h messageService/sync/messageService.cpp)
target_link_libraries(messageService
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
