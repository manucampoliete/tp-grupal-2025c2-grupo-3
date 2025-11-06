#ifndef SERVER_H
#define SERVER_H

#include "../synchronized/matchesMapMonitor.h"
#include "../clientHandling/acceptor.h"

#include <string>

class Server {
private:
    MatchesMapMonitor matchesMapMonitor;
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
     * Destructor: joins the acceptor thread.
     */
    ~Server();
};

#endif  // SERVER_H
