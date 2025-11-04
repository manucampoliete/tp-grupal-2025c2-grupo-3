#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <vector>

#include "../common/commands/move_request_with_id.h"
#include "../common/socket/socket.h"
#include "../common/thread/thread.h"

#include "client_handler.h"
#include "response_queues_monitor.h"
#include "types.h"

class Acceptor: public Thread {
private:
    Socket acceptor;
    Queue<MoveRequestWithID>& client_commands_q;
    ResponseQueuesMonitor& response_queues;
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
    Acceptor(Socket&& acceptor, Queue<MoveRequestWithID>& client_commands_q,
             ResponseQueuesMonitor& response_queues);

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
