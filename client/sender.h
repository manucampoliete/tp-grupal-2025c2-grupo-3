#ifndef SENDER_H
#define SENDER_H

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/utils/move_request.h"
#include "../protocol/client_protocol.h"


/**
 * Sender: hilo que envía comandos del cliente al servidor
 * - Lee comandos de la cola client_requests_q
 * - Los serializa y envía usando ClientProtocol
 */
class Sender: public Thread {
private:
    ClientProtocol& protocol;
    Queue<MoveRequest>& client_requests_q;

public:
    /**
     * Constructor: initializes the Sender with the given parameters.
     * client_requests_q: cola de comandos a enviar
     */
    Sender(ClientProtocol& protocol, Queue<MoveRequest>& client_requests_q);
    
    /**
     * TODO: add proper documentation
     */
    void run() override;

    /**
     * Destructor
     * Nothing special to do
     */
    ~Sender() override = default;
};


#endif // SENDER_H