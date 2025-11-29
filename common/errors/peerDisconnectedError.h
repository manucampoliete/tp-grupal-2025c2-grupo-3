#ifndef PEER_DISCONNECTED_ERROR_H
#define PEER_DISCONNECTED_ERROR_H

#include <stdexcept>

/**
 * Struct that encapsulates a peer disconnected error.
 */

struct PeerDisconnectedError: public std::runtime_error {
    PeerDisconnectedError() : std::runtime_error("Peer is disconnected") {}
};

#endif  // PEER_DISCONNECTED_ERROR_H
