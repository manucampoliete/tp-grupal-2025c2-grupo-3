#include "command.h"

ClientID Command::getClientID() const {
    return client_id;
}

Command::Command(ClientID id) : client_id(id) {}
