# Targets greeter_[async_](client|server)
foreach(_target
        clientAsync)
    add_executable(${_target} shared/consts.h shared/Utils.h shared/Utils.cc shared/ServerStats2.h shared/ServerStats2.cpp client/${_target}.h client/${_target}.cc )
    target_link_libraries(${_target}
            hw_grpc_proto
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})
endforeach()