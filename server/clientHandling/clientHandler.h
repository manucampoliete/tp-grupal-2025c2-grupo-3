#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <memory>

#include "../../common/queue/queue.h"
#include "../../common/socket/socket.h"
#include "../../common/thread/thread.h"
#include "../../common/types/types.h"
#include "../requestsResolving/lobbyResolver.h"
#include "../synchronized/matchesMapMonitor.h"

#include "receiver.h"
#include "sender.h"

/**
 * ClientHandler class: handles communication with a connected client.
 */
class ClientHandler: public Thread {
private:
    Socket peer;
    LobbyResolver lobbyResolver;
    const ClientID clientId;
    std::unique_ptr<Receiver> receiverPtr;
    std::unique_ptr<Sender> senderPtr;

    /**
     * Sets shouldKeepRunning() = false in both the Receiver and the Sender.
     */
    void politeKill();

    /**
     * Does a polite kill, then shuts down and closes the peer socket to unblock
     * any blocking calls in the Receiver and Sender.
     */
    void hardKill();

    /**
     * Handles the lobby phase: processes lobby requests until the client
     * leaves the lobby phase or shouldKeepRunning() becomes false.
     */
    void handleLobbyPhase();

    /**
     * Launches the Sender thread.
     */
    void launchSenderThread();

    /**
     * Launches the Receiver thread (fake start: calls run() directly).
     */
    void fakeLaunchReceiverThread();

public:
    /**
     * Constructor: initializes the ClientHandler with the given parameters.
     * The ClientHandler takes ownership of the peer socket.
     */
    ClientHandler(Socket&& peer, MatchesMapMonitor& matchesMapMonitor, ClientID clientId);

    /**
     * TODO: Document me!
     */
    void run() override;

    /**
     * Kills the Receiver and Sender threads.
     * Does a polite kill, and then a hard kill (this could be changed to just a polite kill).
     * After a call to this method, ClientHandler::isDead() will return true.
     */
    void kill();

    /**
     * Returns true if both the Receiver and Sender threads have finished.
     * Otherwise, returns false.
     */
    bool isDead() const;

    /**
     * Destructor
     * Nothing special to do
     */
    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
