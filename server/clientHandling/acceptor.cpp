#include "acceptor.h"

#include <algorithm>
#include <utility>

#include <sys/socket.h>  // SHUT_RDWR
#include <syslog.h>

#include "../../common/errors/libError.h"

#define FIRST_CLIENT_ID 0

void Acceptor::reapDeadClients() {
    auto it = std::remove_if(clients.begin(), clients.end(), [](const auto& c) {
        return c->isDead();
    });
    clients.erase(it, clients.end());  // cppcheck-suppress missingReturn
}

void Acceptor::fullReapDead() {
    reapDeadClients();
    matchesMapMonitor.reapDeadMatches();
}

void Acceptor::clear() {
    clients.clear();
}

void Acceptor::stop() {
    Thread::stop();  // shouldKeepRunning() = false
    acceptor.shutdown(SHUT_RDWR);
    acceptor.close();
}

Acceptor::Acceptor(const std::string& servname, MatchesMapMonitor& matchesMapMonitor):
    acceptor(servname.c_str()),
    matchesMapMonitor(matchesMapMonitor),
    clients(),
    nextClientId(FIRST_CLIENT_ID) { start(); }

void Acceptor::run() {
    while (shouldKeepRunning()) {
        try {
            Socket peer = acceptor.accept();
            auto c = std::make_unique<ClientHandler>(std::move(peer), matchesMapMonitor,
                                                     nextClientId++);
            fullReapDead();
            clients.push_back(std::move(c));
        } catch (const LibError& err) {
            syslog(LOG_INFO, "[Info] Acceptor: %s", err.what());
            break;
        }
    }
    clear();
}

Acceptor::~Acceptor() {
    stop();
    join();
}
