#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "../common/protocol/dummy_protocol.h"
#include "../common/queue/queue.h"
#include "../common/socket/socket.h"
#include "../common/thread/thread.h"

#include "types.h"

class ClientHandler: public Thread {
private:
    Socket peer;
    DummyProtocol protocol;
    const ClientID client_id;

    /**
     * = Thread::stop().
     */
    void polite_kill();

    /**
     * Does a polite kill, then shuts down and closes the peer socket to unblock
     * any blocking calls.
     */
    void hard_kill();

public:
    /**
     * Constructor: initializes the ClientHandler with the given parameters.
     * The ClientHandler takes ownership of the peer socket.
     */
    ClientHandler(Socket&& peer, ClientID client_id);

    /**
     * Runs the thread.
     * This method will keep receiving messages from the client, echoing them back,
     * until the connection is closed by the client or the server is shutdown.
     */
    virtual void run() override;

    /**
     * Kills the thread.
     * This is a Thread::stop() but more violent. It also shuts down and closes the peer socket,
     * in order to unblock the thread if it is blocked on a recv/send operation.
     * After a call to this method, ClientHandler::is_dead() will return true.
     */
    void kill();

    /**
     * Returns true if the thread is not alive.
     * Otherwise, returns false.
     */
    bool is_dead() const;

    /**
     * Destructor
     * Nothing special to do
     */
    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
