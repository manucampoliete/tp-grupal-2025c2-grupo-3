#include "client.h"
#include <iostream>

/* Client::Client(const char* hostname, const char* servname):
        clprotocol(hostname, servname) {} */

Client::Client(/*const char* hostname, const char* servname*/){
}

int Client::run(int argc, char* argv[]) {
    try {
        QApplication app(argc, argv);

        if (argc < 3) {
            std::cerr << "Uso: ./taller_client <IP> <Puerto>\n";
            return 1;
        }

        std::string ip = argv[1];
        std::string port = argv[2];
        
        Lobby lobby;
        lobby.initiate_connection(ip, port);
        lobby.show();
        return app.exec(); 

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}