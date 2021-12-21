MAIN_IMAGE_NAME=saheru/grpc
BENCH_IMAGE_NAME=saheru/grpc-benchmark

.PHONY: build-image-benchmark build-image-grpc

build-image-benchmark:
	docker build -m 6G -t ${BENCH_IMAGE_NAME} -f benchmark.dockerfile .

build-image-grpc:
	docker build -m 6G -t ${MAIN_IMAGE_NAME} -f grpc.dockerfile .

run_async:
	docker-compose up

run_sync:
	docker-compose -f docker-compose-sync.yml up

exp_async:
	docker-compose --env-file .env.custom up

exp_sync:
	docker-compose --env-file .env.custom -f docker-compose-sync.yml up

exp_clean:
	sudo rm -r container_files/P*

plot:
	python data_analysis/plot_utils.py container_files/default
