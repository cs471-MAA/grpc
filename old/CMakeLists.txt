# Targets greeter_[async_](client|server)
foreach(_target
        greeter_client greeter_server)
    add_executable(${_target} old/${_target}.cc)
    target_link_libraries(${_target}
            hw_grpc_proto
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})
endforeach()