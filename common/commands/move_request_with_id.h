#ifndef MOVE_REQUEST_WITH_ID_H
#define MOVE_REQUEST_WITH_ID_H

#include "move_request.h"
#include "../../server/types.h"

struct MoveRequestWithID {
    MoveRequest move_request;
    ClientID client_id;

    MoveRequestWithID(const MoveRequest& move_request, ClientID client_id)
        : move_request(move_request), client_id(client_id) {}
};

#endif  // MOVE_REQUEST_WITH_ID_H
