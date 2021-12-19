
add_executable(clientAsync shared/consts.h shared/Utils.h shared/Utils.cc shared/ServerStats2.h shared/ServerStats2.cpp client/clientAsync.h client/clientAsync.cc)
target_link_libraries(clientAsync
hw_grpc_proto
${_REFLECTION}
${_GRPC_GRPCPP}
${_PROTOBUF_LIBPROTOBUF})

add_executable(client shared/consts.h shared/Utils.h shared/Utils.cc 
        shared/ServerStats2.h shared/ServerStats2.cpp 
        client/client.h client/client.cc)
target_link_libraries(client
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
