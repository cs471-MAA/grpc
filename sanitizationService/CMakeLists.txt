
add_executable(sanitizationServiceAsync shared/consts.h shared/thread_pool.h sanitizationService/asyncSanitizeMessageHandler.h
        sanitizationService/sanitizationServiceAsync.h sanitizationService/asyncSanitizeMessageHandler.cc
        sanitizationService/sanitizationServiceAsync.cc)
target_link_libraries(sanitizationServiceAsync
        hw_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})