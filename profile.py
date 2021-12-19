"""Cloudlab script to create experiment profile (cluster topology). It will only be used by Cloudlab to setup the cluster nodes."""

# Import the Portal object.
import geni.portal as portal

# Create a portal context.
pc = portal.Context()

# Create a Request object to start building the RSpec.
request = pc.makeRequestRSpec()

def create_node(node_name, ports, entrypoint):
    # Add a Docker container to the request.
    node = request.DockerContainer(node_name)

    # Indicate the image is external
    node.docker_extimage = "saheru/grpc-benchmark:latest"

    # Indicate the dockerfile for the benchmark
    # node.docker_dockerfile = "/local/repository/benchmark.dockerfile"

    # entrypoint
    node.docker_entrypoint = entrypoint

    # can specify cores, ram and disk space
    # node.cores =
    # node.ram =
    # node.disk =

    # TODO: somehow we must be able to specify the ports!

    return node


create_node('mock-database',
            ports='20001:10001',
            entrypoint="/app/cmake/build/mockDatabaseAsync 1 32 100000")
create_node('sanitization-service',
            ports='20003:10003',
            entrypoint="/app/cmake/build/sanitizationServiceAsync 1 32")
create_node('message-service',
            ports='20002:10002',
            entrypoint="/app/cmake/build/messageServiceAsync 1 32")
create_node('client',
            ports='',
            entrypoint="/app/cmake/build/clientAsync 100 TestUser 'Test Message' 100000 0.995")

# Print the RSpec to the enclosing page.
pc.printRequestRSpec(request)
