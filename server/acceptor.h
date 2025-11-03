#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <vector>

#include "../common/socket/socket.h"
#include "../common/thread/thread.h"

#include "client_handler.h"
#include "matches_map_monitor.h"
#include "types.h"

class Acceptor: public Thread {
private:
    Socket acceptor;
    MatchesMapMonitor& matches_map_monitor;
    std::vector<ClientHandler*> clients;
    ClientID next_client_id;

    /**
     * Reaps dead ClientHandlers: joins and deletes them, and removes them from the clients vector.
     */
    void reap_dead();

    /**
     * Clears all ClientHandlers: kills, joins, and deletes them, and clears the clients vector.
     */
    void clear();

public:
    /**
     * Constructor: initializes the acceptor with the given parameters.
     * Takes the ownership of the acceptor Socket.
     */
    Acceptor(const std::string& servname, MatchesMapMonitor& matches_map_monitor);

    /**
     * Main acceptor logic: accepts new clients and spawns a ClientHandler for each one.
     * Every time a new client is accepted, Acceptor::reap_dead() is called. When a call to
     * Acceptor::stop() is made, stops accepting new clients and calls Acceptor::clear() to clean up
     * all ClientHandlers.
     */
    void run() override;

    /**
     * Stops the acceptor: Thread::should_keep_running() will be false.
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
