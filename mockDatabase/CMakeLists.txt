


add_executable(mockDatabaseAsync  mockDatabase/asyncRequestHandler.h shared/thread_pool.h
        mockDatabase/mockDatabaseAsync.h mockDatabase/mockDatabaseAsync.cc)
target_link_libraries(mockDatabaseAsync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

add_executable(mockDatabaseSync mockDatabase/mockDatabaseSync.h mockDatabase/mockDatabaseSync.cc)
target_link_libraries(mockDatabaseSync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})