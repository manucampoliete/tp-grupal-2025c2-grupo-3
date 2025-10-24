#include "client.h"

#include <iostream>
#include <utility>

#include "../common/protocol/dummy_protocol.h"
#include "../common/socket/socket.h"

Client::Client(std::string&& server_hostname, std::string&& server_servname):
        server_hostname(std::move(server_hostname)), server_servname(std::move(server_servname)) {}

int Client::run() {
    Socket peer(server_hostname.c_str(), server_servname.c_str());
    DummyProtocol protocol(peer);

    std::string recv_msg = protocol.recv_message();
    std::cout << "Received from server: " << recv_msg << "\n";

    std::string send_msg = "Hello, server! Client here.";
    std::cout << "Sending to server: " << send_msg << "\n";
    protocol.send_message(send_msg);

    return EXIT_SUCCESS;
}

Client::~Client() {}
