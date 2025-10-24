#ifndef SERVER_H
#define SERVER_H

#include <string>

class Server {
private:
    const std::string servname;

    /**
     * Disable copy semantics (not needed and error-prone)
     */
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

public:
    /**
     * Constructor: takes the server name (to bind the socket to)
     */
    explicit Server(std::string&& servname);

    /**
     * Enable move semantics (default implementations are fine)
     */
    Server(Server&&) = default;
    Server& operator=(Server&&) = default;

    /**
     * Runs the server: accepts only one client and talks with it a little bit.
     * Returns EXIT_SUCCESS if everything went fine, or EXIT_FAILURE otherwise.
     */
    int run();

    /**
     * Destructor
     * Nothing special to do
     */
    ~Server();
};

#endif  // SERVER_H
