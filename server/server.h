#ifndef SERVER_H
#define SERVER_H

#include <string>

#include "clientHandling/acceptor.h"
#include "synchronized/matchesMapMonitor.h"

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
     * Runs the server: waits for the
     * termination key to shut it down.
     */
    void run();

    /**
     * Destructor: destroys the acceptor and the matches map monitor.
     */
    ~Server();
};

#endif  // SERVER_H
