#include "acceptor.h"

#include "../common/socket/liberror.h"

#include <algorithm>
#include <utility>
#include <sys/socket.h>  // SHUT_RDWR
#include <syslog.h>
#include <sstream>
#include <iostream>

#define FIRST_CLIENT_ID 0

void Acceptor::reap_dead() {
    auto it = std::remove_if(clients.begin(), clients.end(), [](ClientHandler* c) {
        bool is_dead = c->is_dead();
        if (is_dead) {
            c->join();
            delete c;
        }
        return is_dead;
    });
    clients.erase(it, clients.end());
}

void Acceptor::clear() {
    for (ClientHandler* client: clients) {
        client->kill();
        client->join();
        delete client;
    }
    clients.clear();
}

Acceptor::Acceptor(Socket&& acceptor):
        acceptor(std::move(acceptor)),
        clients(),
        next_client_id(FIRST_CLIENT_ID) {}

void Acceptor::run() {
    while (should_keep_running()) {
        try {
            Socket peer = acceptor.accept();
            std::ostringstream oss;
            oss << "[Server] Accepted new client with ID: " << next_client_id << std::endl;
            std::cout << oss.str();  // Atomic
            ClientHandler* c = new ClientHandler(std::move(peer), next_client_id++);
            reap_dead();
            clients.push_back(c);
            c->start();
        } catch (const LibError& err) {
            syslog(LOG_INFO, "[Info] Acceptor: %s", err.what());
            break;
        }
    }
    clear();
}

void Acceptor::stop() {
    Thread::stop();  // should_keep_running() = false
    acceptor.shutdown(SHUT_RDWR);
    acceptor.close();
}

Acceptor::~Acceptor() {}
