#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <memory>
#include <optional>

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
    std::optional<Sender> sender;
    std::optional<Receiver> receiver;

    /**
     * Does a polite kill: stops the ClientHandler-Receiver thread and the Sender thread (if it was
     * launched).
     */
    void politeKill();

    /**
     * Does a polite kill, then shuts down and closes the peer socket to unblock
     * any blocking calls in the ClientHandler-Receiver and Sender (if it was launched) threads.
     */
    void hardKill();

    /**
     * Handles the lobby phase: processes lobby requests until the client
     * leaves the lobby phase or shouldKeepRunning() becomes false.
     */
    void handleLobbyPhase();

public:
    /**
     * Constructor: initializes the ClientHandler with the given parameters.
     * The ClientHandler takes ownership of the peer socket.
     */
    ClientHandler(Socket&& peer, MatchesMapMonitor& matchesMapMonitor, ClientID clientId);

    /**
     * Joins the ClientHandler-Receiver thread and the Sender thread (if it was launched).
     */
    void join() override;

    /**
     * Kills the ClientHandler-Receiver thread and the Sender thread (if it was launched).
     * Does a polite kill, and then a hard kill (this could be changed to just a polite kill).
     * After a call to this method, ClientHandler::isDead() will return true.
     */
    void kill();

    /**
     * Returns true if both the ClientHandler-Receiver and Sender threads have finished.
     * Otherwise, returns false.
     * Note: if the Sender thread was not launched, only checks the ClientHandler-Receiver thread.
     */
    bool isDead() const;

    /**
     * Main ClientHandler logic: handles the lobby phase, and once it is complete launches the
     * Sender thread and runs the Receiver logic in this same thread.
     */
    void run() override;

    /**
     * Destructor
     * Nothing special to do
     */
    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
