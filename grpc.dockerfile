FROM ubuntu:latest

WORKDIR /grpc

# install dependecies
RUN export DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
    && apt-get install -y --no-install-recommends git cmake build-essential autoconf libtool pkg-config net-tools linux-tools-common linux-tools-generic

# clone gRPC
RUN git config --global http.sslverify false \
    && git clone --recurse-submodules -b v1.43.0 https://github.com/grpc/grpc.git /grpc

# compile gRPC
RUN cd /grpc \
    && mkdir -p cmake/build \
    && cd cmake/build \
    && cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      ../.. \
    && make -j 6 \
    && make install

# clean
RUN rm -r /grpc
