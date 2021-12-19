
add_executable(sanitizationServiceAsync shared/ServerStats2.h shared/ServerStats2.cpp shared/consts.h shared/thread_pool.h shared/Utils.h shared/Utils.cc
        sanitizationService/asyncSanitizeMessageHandler.h
        sanitizationService/sanitizationServiceAsync.h sanitizationService/asyncSanitizeMessageHandler.cc
        sanitizationService/sanitizationServiceAsync.cc)
target_link_libraries(sanitizationServiceAsync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})


add_executable(sanitizationService shared/ServerStats2.h shared/ServerStats2.cpp shared/consts.h shared/Utils.h shared/Utils.cc
        sanitizationService/sync/sanitizationService.h sanitizationService/sync/sanitizationService.cc
        shared/ServerStats2.h shared/ServerStats2.cpp)
target_link_libraries(sanitizationService
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
