# Targets greeter_[async_](client|server)
foreach(_target
        asyncClient2)
    add_executable(${_target} shared/consts.h client/${_target}.h client/${_target}.cc )
    target_link_libraries(${_target}
            hw_grpc_proto
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})
endforeach()