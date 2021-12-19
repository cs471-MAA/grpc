


add_executable(mockDatabaseAsync shared/consts.h shared/thread_pool.h shared/HashNode.h shared/HashMap.h
        shared/asyncHandler.h shared/Utils.h shared/Utils.cc shared/ServerStats2.h shared/ServerStats2.cpp
        mockDatabase/asyncFindLastMessageHandler.h
        mockDatabase/asyncSaveMessageHandler.h mockDatabase/mockDatabaseAsync.h mockDatabase/mockDatabaseAsync.cc)
target_link_libraries(mockDatabaseAsync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

add_executable(mockDatabase shared/HashNode.h shared/HashMap.h mockDatabase/sync/mockDatabase.h mockDatabase/sync/mockDatabase.cc)
target_link_libraries(mockDatabase
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})