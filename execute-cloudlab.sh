#!/bin/bash

if [ $# -lt 1 ] || [ "$1" = "-h" ] || [ "$1" = "-?" ] || [ "$1" = "--help" ] ; then
     cat <<__EOT__

     Usage: $(basename "$0") <SYNC | ASYNC> <.env file>

__EOT__
    exit 1
fi


[ ! -f "$HOME/.ssh/id_cloudlab" ] && {
    echo 'please setup your Cloudlab cluster and ssh credentials. Save your private key as ~/.ssh/id_cloudlab' >&2
    exit 1
}

USERNAME=saheru
SYNC="$1"  # 1 means SYNC, 0 means ASYNC
ENV_FILE="$2"

[ -z "$SYNC" ] && {
    echo 'you did not provide SYNC / ASYNC as the 1st argument. Defaulting to ASYNC' >&2
    SYNC=0
}
[ -z "$ENV_FILE" ] && {
    echo 'you did not provide the .env file as the 2nd argument. Defaulting to .env.custom' >&2
    ENV_FILE='.env.custom'
}

[ ! -f "$ENV_FILE" ] && {
    echo "$ENV_FILE does not exist" >&2
    exit 1
}

[ "$SYNC" = "SYNC" ] && SYNC=1 || SYNC=0

[ $SYNC -eq 1 ] && echo 'running SYNC'
[ $SYNC -eq 0 ] && echo 'running ASYNC'

[ $SYNC -eq 1 ] && DOCKER_COMPOSE='docker-compose-swarm-sync.yml'
[ $SYNC -eq 0 ] && DOCKER_COMPOSE='docker-compose-swarm-async.yml'

source "$ENV_FILE"
OUT_DIR="/users/$USERNAME/grpc/container_files/$STATSDIR"

eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_cloudlab

function node() {
    echo "node$1.grpc-benchmark.cloudsuite3-pg0.apt.emulab.net"
}

# update git
for nodei in $(seq 0 3); do
    echo "updating node $nodei git.."
    node_name="$(node $nodei)"
    git_commands='\
    git config --global credential.helper store && \
    git clone https://github.com/cs471-MAA/grpc.git || cd grpc && git pull; \
    sudo docker pull saheru/grpc-benchmark; \
    mkdir -p '"$OUT_DIR"
    ssh -p 22 $USERNAME@"$node_name" "$git_commands"
done

# deploy stack (node #3 is the manager)
ENV_VARS="$(grep "^[A-Za-z]" "$ENV_FILE" | tr "\n" " ")"
deploy_pre_commands='cd grpc'
deploy_post_commands='sleep 1 && sudo docker node ls && sudo docker service ls'
ssh -p 22 $USERNAME@"$(node 3)" "$deploy_pre_commands"' && env '"$ENV_VARS"' sudo -E docker stack deploy -c '"$DOCKER_COMPOSE"' grpc && '"$deploy_post_commands" </dev/null

# TODO: instead of waiting, client should stop automatically..
echo 'sleeping for 10s..'
sleep 10

# stop stack
ssh -p 22 $USERNAME@"$(node 3)" 'sudo docker stack rm grpc' </dev/null

# retrieve results
mkdir -p container_files/cluster/"$STATSDIR"
cp "$ENV_FILE" container_files/cluster/"$STATSDIR"
for nodei in $(seq 0 3); do
    echo "retrieving data from node $nodei .."
    node_name="$(node $nodei)"
    scp -r $USERNAME@"$node_name":"$OUT_DIR" container_files/cluster/
done

echo 'DONE'
exit 0
