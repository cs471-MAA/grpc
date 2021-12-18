MAIN_IMAGE_NAME=saheru/grpc

.PHONY: build-image-benchmark build-image-grpc

build-image-grpc:
	docker build -m 6G -t ${MAIN_IMAGE_NAME} -f grpc.dockerfile .
