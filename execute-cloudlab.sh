#!/bin/bash

[ ! -f "$HOME/.ssh/id_cloudlab" ] && {
    echo 'please setup your Cloudlab cluster and ssh credentials. Save your private key as ~/.ssh/id_cloudlab' >&2
    exit 1
}

USERNAME=saheru

source .env

eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_cloudlab

function node() {
    echo "node$1.grpc-benchmark.cloudsuite3-pg0.apt.emulab.net"
}

# update git
for nodei in $(seq 0 3); do
    echo "updating node $nodei git.."
    node_name="$(node $nodei)"
    ssh -p 22 $USERNAME@"$node_name" 'git clone https://github.com/cs471-MAA/grpc.git || cd grpc && git pull' </dev/null
done

# deploy stack (node #3 is the manager)
ssh -p 22 $USERNAME@"$(node 3)" 'env `cat .env | grep "^[A-Za-z]"` sudo -E docker stack deploy -c docker-compose.yml grpc' </dev/null

# TODO: instead of waiting, client should stop automatically..
echo 'sleeping for 10s..'
sleep 10

# stop stack
ssh -p 22 $USERNAME@"$(node 3)" 'sudo docker stack rm grpc' </dev/null

# retrieve results
mkdir -p container_files/cluster
for nodei in $(seq 0 3); do
    echo "retrieving data from node $nodei .."
    node_name="$(node $nodei)"
    scp -r $USERNAME@"$node_name":/users/$USERNAME/grpc/container_files/default/ container_files/cluster
done

echo 'Done'
exit 0
