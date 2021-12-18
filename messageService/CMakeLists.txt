# Targets greeter_[async_](client|server)

add_executable(messageService messageService/messageService.h messageService/asyncClient.h
        messageService/messageService.cc messageService/asyncClient.cc)
target_link_libraries(messageService
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})