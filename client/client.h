#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client {
private:
    const std::string server_hostname;
    const std::string server_servname;

    /**
     * Disable copy semantics (not needed and error-prone)
     */
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

public:
    /**
     * Constructor: takes the server hostname and service name (to connect to)
     */
    Client(std::string&& server_hostname, std::string&& server_servname);

    /**
     * Enable move semantics (default implementations are fine)
     */
    Client(Client&&) = default;
    Client& operator=(Client&&) = default;

    /**
     * Runs the client: connects to the server and talks with it a little bit.
     * Returns EXIT_SUCCESS if everything went fine, or EXIT_FAILURE otherwise.
     */
    int run();

    /**
     * Destructor
     * Nothing special to do
     */
    ~Client();
};

#endif  // CLIENT_H
