#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <memory>
#include <string>
#include <vector>

#include "../../common/socket/socket.h"
#include "../../common/thread/thread.h"
#include "../../common/types/types.h"
#include "../synchronized/matchesMapMonitor.h"

#include "clientHandler.h"

/**
 * Acceptor class: accepts new client connections and spawns a ClientHandler for each one.
 */
class Acceptor: public Thread {
private:
    Socket acceptor;
    MatchesMapMonitor& matchesMapMonitor;
    std::vector<std::unique_ptr<ClientHandler>> clients;
    ClientID nextClientId;

    /**
     * Reaps dead ClientHandlers: joins and deletes them, and removes them from the clients vector.
     */
    void reapDeadClients();

    /**
     * Reaps dead ClientHandlers and reaps finished matches in the MatchesMapMonitor.
     */
    void fullReapDead();

    /**
     * Clears all ClientHandlers: kills, joins, and deletes them, and clears the clients vector.
     */
    void clear();

public:
    /**
     * Constructor: initializes the acceptor with the given parameters.
     */
    Acceptor(const std::string& servname, MatchesMapMonitor& matchesMapMonitor);

    /**
     * Main acceptor logic: accepts new clients and spawns a ClientHandler for each one.
     * Every time a new client is accepted, Acceptor::reapDead() is called. When a call to
     * Acceptor::stop() is made, stops accepting new clients and calls Acceptor::clear() to clean up
     * all ClientHandlers.
     */
    void run() override;

    /**
     * Stops the acceptor: Thread::shouldKeepRunning() will be false.
     * If blocked on Socket::accept(), the acceptor socket is shutdown and closed to unblock it.
     */
    void stop() override;

    /**
     * Destructor
     * Nothing special to do, ClientHandlers are cleaned up in Acceptor::run() when the acceptor is
     * stopped.
     */
    ~Acceptor() override;
};

#endif  // ACCEPTOR_H
