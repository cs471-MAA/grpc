FROM saheru/grpc:latest

WORKDIR /app
COPY . /app

RUN cd /app/ \
    && mkdir -p build \
    && cd build \
    && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. \
    && make -j 4
