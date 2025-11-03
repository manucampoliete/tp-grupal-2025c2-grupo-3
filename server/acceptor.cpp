#include "acceptor.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>

#include <sys/socket.h>  // SHUT_RDWR
#include <syslog.h>

#include "../common/socket/liberror.h"

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
    for (ClientHandler* c: clients) {
        c->kill();
        c->join();
        delete c;
    }
    clients.clear();
}

Acceptor::Acceptor(const std::string& servname, MatchesMapMonitor& matches_map_monitor):
        acceptor(servname.c_str()),
        matches_map_monitor(matches_map_monitor),
        clients(),
        next_client_id(FIRST_CLIENT_ID) {}

void Acceptor::run() {
    while (should_keep_running()) {
        try {
            Socket peer = acceptor.accept();
            ClientHandler* c = new ClientHandler(std::move(peer), matches_map_monitor, next_client_id++);
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
