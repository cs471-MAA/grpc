# Setup

```shell
make
docker compose up
```

# Running in a cluster

You will need at least one node that will act as a **manager**. In that node execute:

```shell
docker swarm init --advertise-addr <ip-addr>
```

Choose your `<ip-addr>` using `ifconfig`. You will then get instructions on a command to execute in the other nodes so Docker knows about their existance:

```shell
docker swarm join --token <token> <ip-addr>:<port>
```

You can check the status of the nodes by running `docker node ls` in the **manager** node. After joining all nodes, we can deploy the stack ([official Docker tutorial](https://docs.docker.com/engine/swarm/stack-deploy/)). To do that, execute the following in the **manager** node:

```shell
git clone https://github.com/cs471-MAA/grpc.git
cd grpc
env `cat .env | grep '^[A-Za-z]'` sudo -E docker stack deploy -c docker-compose.yml grpc
```

You can now list the running services with `docker stack services grpc`. You can also see the hostname aliases with:

```shell
docker service inspect --format='{{json .Spec.TaskTemplate.ContainerSpec.Hostname}}' <service-name>
```

Finally, to bring the stack down use:

```shell
docker stack rm grpc
```

And to stop docker swarm:

```shell
docker swarm leave --force
```

# Old

```
# gRPC C++ Examples

- **[Hello World][]!** Eager to run your first gRPC example? You'll find
  instructions for building gRPC and running a simple "Hello World" app in [Quick Start][].
- **[Route Guide][].** For a basic tutorial on gRPC see [gRPC Basics][].

For information about the other examples in this directory, see their respective
README files.

[gRPC Basics]: https://grpc.io/docs/languages/cpp/basics
[Hello World]: helloworld
[Quick Start]: https://grpc.io/docs/languages/cpp/quickstart
[Route Guide]: route_guide
```
