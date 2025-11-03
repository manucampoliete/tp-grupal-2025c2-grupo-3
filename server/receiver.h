#ifndef RECEIVER_H
#define RECEIVER_H

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"

#include "../common/protocol/server_protocol.h"
#include "matches_map_monitor.h"
#include "types.h"
#include "sender.h"

class Receiver: public Thread {
private:
    ServerProtocol& protocol;
    MatchesMapMonitor& matches_map_monitor;
    const ClientID client_id;
    Sender& sender;
    Queue<std::unique_ptr<Command>>* client_commands_q;
    Queue<Snapshot>* responses_q;
    
public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     */
    Receiver(ServerProtocol& protocol, MatchesMapMonitor& matches_map_monitor, ClientID client_id, Sender& sender);

    /**
     * TODO: add proper documentation
     */
    void run() override;

    /**
     * Sets the commands queue for the receiver.
     */
    void set_client_commands_queue(Queue<std::unique_ptr<Command>>* client_commands_q);

    /**
     * Sets the responses queue for the receiver.
     */
    void set_responses_queue(Queue<Snapshot>* responses_q);

    /**
     * Destructor
     * Nothing special to do
     */
    ~Receiver() override = default;
};

#endif  // RECEIVER_H
