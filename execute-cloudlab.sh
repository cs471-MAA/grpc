#!/bin/bash

[ ! -f "$HOME/.ssh/id_cloudlab" ] && {
    echo 'please setup your Cloudlab cluster and ssh credentials. Save your private key as ~/.ssh/id_cloudlab' >&2
    exit 1
}

source .env

eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_cloudlab

BENCH_IMAGE_NAME='saheru/grpc-benchmark:latest'
setup_commands="sudo docker pull $BENCH_IMAGE_NAME"
setup_commands="${setup_commands} && sudo docker container stop \`sudo docker container ls -aq\`"
setup_commands="${setup_commands} && sudo docker container prune -f"

while read -r dest ports entrypoint; do
    if [ "$ports" = ":" ]; then
        docker_command="sudo docker run --rm -v stats_files:/app/stats_files:rw $BENCH_IMAGE_NAME $entrypoint"
    else
        docker_command="sudo docker run --rm  -v stats_files:/app/stats_files:rw -p $ports $BENCH_IMAGE_NAME $entrypoint"
    fi
    echo "$dest: running docker command: $docker_command"
    ssh -f -p 22 "$dest" "$setup_commands && $docker_command" < /dev/null
done <<EOF
saheru@apt048.apt.emulab.net 20001:10001 /app/cmake/build/mockDatabaseAsync ${W_MSG} ${t_MSG} ${s_MSG}
saheru@apt052.apt.emulab.net 20003:10003 /app/cmake/build/sanitizationServiceAsync ${W_SANIT} ${t_SANIT} ${s_SANIT}
saheru@apt049.apt.emulab.net 20002:10002 /app/cmake/build/messageServiceAsync ${W_MSG} ${t_MSG} ${s_MSG}
saheru@apt054.apt.emulab.net : /app/cmake/build/clientAsync ${N} ${t_DELAY} ${s_DELAY} ${P} ${SEED}
EOF
