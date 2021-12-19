//
// Created by adrien on 19.12.21.
//

#include "client.h"
#include "../shared/consts.h"

int main(int argc, char** argv) {
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).

    Client client(grpc::CreateChannel(M_MESSAGE_SERVICE_SYNC_SOCKET_ADDRESS, grpc::InsecureChannelCredentials()));
    std::string user("world");
    std::string reply = client.findLastMessage(user);
    std::cout << "Greeter received: " << reply << std::endl;

    return 0;
}