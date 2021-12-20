# Setup

```shell
make
docker compose up
```

# Running in a cluster

You will need at least one node that will act as a manager. In that node execute:

```shell
docker swarm init --advertise-addr <ip-addr>
```

Choose your `<ip-addr>` using `ifconfig`. You will then get instructions on a command to execute in the other nodes so Docker knows about their existance:

```shell
docker swarm join --token <token> <ip-addr>:<port>
```

After joining all nodes, we can deploy the stack ([official Docker tutorial](https://docs.docker.com/engine/swarm/stack-deploy/)). To do that, copy the toplevel `docker-compose.yml` with a command like `scp` or `git clone` into the **manager** node and execute:

```shell
docker stack deploy --compose-file docker-compose.yml grpc
```

You can now list the running services with `docker stack services grpc`.

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
