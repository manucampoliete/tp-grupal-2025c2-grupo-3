#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "../common/queue/queue.h"
#include "../common/socket/socket.h"
#include "../common/thread/thread.h"

#include "receiver.h"
#include "matches_map_monitor.h"
#include "types.h"

/**
 * This class exposes an API similar to Thread, but it is not a Thread itself.
 * It contains two threads: a Receiver and a Sender.
 * ClientHandler::start(), ClientHandler::join(), ClientHandler::kill(), and
 * ClientHandler::is_dead() control both the Receiver and the Sender.
 */
class ClientHandler {
private:
    Socket peer;
    ServerProtocol protocol;
    Sender sender;
    Receiver receiver;
    const ClientID client_id;

    /**
     * Sets should_keep_running() = false in both the Receiver and the Sender.
     */
    void polite_kill();

    /**
     * Does a polite kill, then shuts down and closes the peer socket to unblock
     * any blocking calls in the Receiver and Sender.
     */
    void hard_kill();

public:
    /**
     * Constructor: initializes the ClientHandler with the given parameters.
     * The ClientHandler takes ownership of the peer socket.
     */
    ClientHandler(Socket&& peer, MatchesMapMonitor& matches_map_monitor, ClientID client_id);

    /**
     * Starts the Receiver and Sender threads.
     */
    void start();

    /**
     * Joins the Receiver and Sender threads (waits for them to finish).
     */
    void join();

    /**
     * Kills the Receiver and Sender threads.
     * Does a polite kill, and then a hard kill (this could be changed to just a polite kill).
     * After a call to this method, ClientHandler::is_dead() will return true.
     */
    void kill();

    /**
     * Returns true if both the Receiver and Sender threads have finished.
     * Otherwise, returns false.
     */
    bool is_dead() const;

    /**
     * Destructor: removes the subscribed response queue from the ResponseQueuesMonitor.
     */
    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
