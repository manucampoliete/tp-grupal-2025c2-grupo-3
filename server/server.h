#ifndef SERVER_H
#define SERVER_H

#include <string>

#include "acceptor.h"
#include "matches_map_monitor.h"

class Server {
private:
    MatchesMapMonitor matches_map_monitor;
    Acceptor acceptor;

    /**
     * Disable copy semantics (not needed and error-prone)
     */
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

public:
    /**
     * Constructor: takes the service name (to bind the socket to)
     */
    explicit Server(const std::string& servname);

    /**
     * Enable move semantics (default implementations are fine)
     */
    Server(Server&&) = default;
    Server& operator=(Server&&) = default;

    /**
     * Runs the server: starts acceptor thread, waits for the
     * termination key, and then stops it.
     * Returns EXIT_SUCCESS if everything went fine.
     */
    int run();

    /**
     * Destructor: joins the game and acceptor threads.
     */
    ~Server();
};

#endif  // SERVER_H
