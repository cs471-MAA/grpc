

#[[
add_executable(mockDatabaseAsync shared/HashNode.h shared/Utils.h shared/Utils.cc shared/HashMap.h 
mockDatabase/mockDatabaseAsync.h mockDatabase/mockDatabaseAsync.cc
shared/ServerStats2.h shared/ServerStats2.cpp)
target_link_libraries(mockDatabaseAsync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
]]

add_executable(mockDatabase shared/HashNode.h shared/Utils.h shared/Utils.cc shared/HashMap.h 
        mockDatabase/mockDatabase.h mockDatabase/mockDatabase.cc
        shared/ServerStats2.h shared/ServerStats2.cpp)
target_link_libraries(mockDatabase
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})