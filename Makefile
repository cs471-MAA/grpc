MAIN_IMAGE_NAME=saheru/grpc
BENCH_IMAGE_NAME=saheru/grpc-benchmark

.PHONY: build-image-benchmark build-image-grpc

build-image-benchmark:
	docker build -m 6G -t ${BENCH_IMAGE_NAME} -f benchmark.dockerfile .

build-image-grpc:
	docker build -m 6G -t ${MAIN_IMAGE_NAME} -f grpc.dockerfile .
