#include "acceptor.h"
#include "../../common/errors/libError.h"

#include <algorithm>
#include <utility>
#include <sys/socket.h>  // SHUT_RDWR
#include <syslog.h>

#define FIRST_CLIENT_ID 0

void Acceptor::reapDeadClients() {
    auto it = std::remove_if(clients.begin(), clients.end(), [](const auto& c) {
        bool isDead = c->isDead();
        if (isDead) {
            c->join();
        }
        return isDead;
    });
    clients.erase(it, clients.end());
}

void Acceptor::reapDead() {
    reapDeadClients();
    // matchesMapMonitor.reapDeadMatches();
}

void Acceptor::clear() {
    for (const auto& c : clients) {
        c->kill();
        c->join();
    }
    clients.clear();
}

Acceptor::Acceptor(const std::string& servname, MatchesMapMonitor& matchesMapMonitor):
        acceptor(servname.c_str()),
        matchesMapMonitor(matchesMapMonitor),
        clients(),
        nextClientId(FIRST_CLIENT_ID) {}

void Acceptor::run() {
    while (shouldKeepRunning()) {
        try {
            Socket peer = acceptor.accept();
            auto c = std::make_unique<ClientHandler>(std::move(peer), matchesMapMonitor, nextClientId++);
            reapDead();
            clients.push_back(std::move(c));
            clients.back()->start();
        } catch (const LibError& err) {
            syslog(LOG_INFO, "[Info] Acceptor: %s", err.what());
            break;
        }
    }
    clear();
}

void Acceptor::stop() {
    Thread::stop();  // shouldKeepRunning() = false
    acceptor.shutdown(SHUT_RDWR);
    acceptor.close();
}

Acceptor::~Acceptor() {}
