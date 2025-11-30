#include "server.h"

#include <iostream>

#define END_SERVER_KEY 'q'
#define CONFIG_YAML_PATH "config.yaml"

Server::Server(const std::string& servname):
    config(ConfigLoader::Load(CONFIG_YAML_PATH)),
    matchesMapMonitor(config),
    acceptor(servname, matchesMapMonitor, config.carsInfo) {}

void Server::run() {
    while (std::cin.get() != END_SERVER_KEY) {}
}

Server::~Server() {}
