#include "command.h"

ClientID Command::getClientID() const { return clientId; }

Command::Command(ClientID clientId): clientId(clientId) {}
