#!/bin/env sh
# shell script that installs docker on the nodes in the cluster

[ ! -f "$HOME/.ssh/id_cloudlab" ] && {
    echo 'please setup your Cloudlab cluster and ssh credentials. Save your private key as ~/.ssh/id_cloudlab' >&2
    exit 1
}

eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_cloudlab

commands=' \
sudo apt-get update && \
    sudo apt-get install -y --no-install-recommends ca-certificates curl gnupg lsb-release && \
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg && \
    echo "deb [arch=`dpkg --print-architecture` signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu `lsb_release -cs` stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null && \
    sudo apt-get update && sudo apt-get install -y --no-install-recommends docker-ce docker-ce-cli containerd.io && \
    echo DONE
'

function node() {
    echo "node$1.grpc-benchmark.cloudsuite3-pg0.apt.emulab.net"
}

for i in $(seq 0 3); do
    echo "node $i"
    ssh -p 22 "$(node $i)" "$commands" </dev/null &
done

wait
exit 0
