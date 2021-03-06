FROM saheru/grpc:latest

WORKDIR /app
COPY . /app

RUN apt-get install -y iproute2

RUN cd /app/ \
    && mkdir -p cmake/build \
    && cd cmake/build \
    && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../.. \
    && make -j 4

