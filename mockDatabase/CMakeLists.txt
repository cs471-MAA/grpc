# Targets greeter_[async_](client|server)
foreach(_target
        mockDatabase)
    add_executable(${_target} mockDatabase/${_target}.h shared/thread_pool.h mockDatabase/${_target}.cc)
    target_link_libraries(${_target}
            hw_grpc_proto
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})
endforeach()