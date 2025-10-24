#include "client.h"

#include <iostream>
#include <utility>

#include "../common/protocol/dummy_protocol.h"
#include "../common/socket/socket.h"

Client::Client(std::string&& server_hostname, std::string&& server_servname):
        server_hostname(std::move(server_hostname)), server_servname(std::move(server_servname)) {}

int Client::run() {
    Socket socket(server_hostname.c_str(), server_servname.c_str());
    DummyProtocol protocol(socket);

    while (true) {
        try {
            std::string msg;
            std::cout << "[Client] Enter a message to send to the server (or 'exit' to quit): ";
            std::getline(std::cin, msg);

            if (msg == "exit") {
                std::cout << "[Client] Exiting." << std::endl;
                break;
            }

            protocol.send_message(msg);
            std::cout << "[Client] Message sent to server: " << msg << "\n";

            std::string response = protocol.recv_message();
            std::cout << "[Client] Response from server: " << response << "\n";

            if (response.empty()) {
                std::cout << "[Client] Server closed the connection." << std::endl;
                break;
            }

        } catch (const std::exception& e) {
            std::cerr << "[Client] Error: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

Client::~Client() {}
