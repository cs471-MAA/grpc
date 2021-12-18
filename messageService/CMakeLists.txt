# Targets greeter_[async_](client|server)
foreach(_target
        messageService)
    add_executable(${_target} messageService/${_target}.cc)
    target_link_libraries(${_target}
            hw_grpc_proto
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})
endforeach()