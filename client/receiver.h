#ifndef RECEIVER_H
#define RECEIVER_H

#include "../common/messages/snapshot.h"
#include "../common/protocol/protocolConstants.h"
#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/types/types.h"
#include "../common/utils/activeDirections.h"
#include "../protocol/client_protocol.h"


class GameHandler;

/**
 * Receiver: hilo que recibe mensajes del servidor
 * - Lee mensajes del servidor usando ClientProtocol
 * - Los deserializa y notifica al GameHandler
 */
class Receiver: public Thread {
private:
    ClientProtocol& protocol;
    Queue<Snapshot>& server_snapshots_q;
    GameHandler& game_handler;

public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     * server_snapshots_q: cola de snapshots recibidos
     * game_handler: referencia al GameHandler para notificar eventos
     */
    Receiver(ClientProtocol& protocol, Queue<Snapshot>& server_snapshots_q,
             GameHandler& game_handler);

    /**
     * TODO: Add proper documentation
     */
    void run() override;

    /**
     * Destructor
     * Nothing special to do
     */
    ~Receiver() override = default;
};

#endif  // RECEIVER_H
