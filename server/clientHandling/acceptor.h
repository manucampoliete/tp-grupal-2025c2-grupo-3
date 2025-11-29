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
    const std::vector<CarInfo>& carsInfo;
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
     * Clears the clients vector (kills, joins, and deletes all ClientHandlers).
     */
    void clear();

    /**
     * Stops the acceptor: Thread::shouldKeepRunning() will be false.
     * If blocked on Socket::accept(), the acceptor socket is shutdown and closed to unblock it.
     */
    void stop() override;

public:
    /**
     * Constructor: initializes the acceptor with the given parameters.
     */
    Acceptor(const std::string& servname, MatchesMapMonitor& matchesMapMonitor, const std::vector<CarInfo>& carsInfo);

    /**
     * Main acceptor logic: accepts new clients and spawns a ClientHandler for each one.
     * Every time a new client is accepted, Acceptor::fullReapDead() is called. When destructed,
     * stops accepting new clients and calls Acceptor::clear() to clean up
     * all ClientHandlers.
     */
    void run() override;

    /**
     * Destructor
     * Stops the acceptor and joins the thread.
     */
    ~Acceptor() override;
};

#endif  // ACCEPTOR_H
