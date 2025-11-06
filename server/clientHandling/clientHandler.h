#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "receiver.h"
#include "sender.h"

#include "../requestsResolving/lobbyResolver.h"
#include "../synchronized/matchesMapMonitor.h"

#include "../../common/queue/queue.h"
#include "../../common/socket/socket.h"
#include "../../common/thread/thread.h"
#include "../../common/types/types.h"

/**
 * ClientHandler class: handles communication with a connected client.
 */
class ClientHandler : public Thread {
private:
    Socket peer;
    LobbyResolver lobbyResolver;
    const ClientID client_id;
    std::unique_ptr<Receiver> receiver_ptr;
    std::unique_ptr<Sender> sender_ptr;

    /**
     * Sets shouldKeepRunning() = false in both the Receiver and the Sender.
     */
    void politeKill();

    /**
     * Does a polite kill, then shuts down and closes the peer socket to unblock
     * any blocking calls in the Receiver and Sender.
     */
    void hardKill();

public:
    /**
     * Constructor: initializes the ClientHandler with the given parameters.
     * The ClientHandler takes ownership of the peer socket.
     */
    ClientHandler(Socket&& peer, MatchesMapMonitor& matches_map_monitor, ClientID client_id);

    /**
     * TODO: Document me!
     */
    void run() override;

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
    bool isDead() const;

    /**
     * Destructor: removes the subscribed response queue from the ResponseQueuesMonitor.
     */
    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
