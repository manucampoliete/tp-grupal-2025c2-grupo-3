#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <SDL2pp/SDL.hh>
#include <cstdint>
#include "client.h"
#include "car.h"

using namespace SDL2pp;

class EventHandler {
private:
    Client& client;  // referencia al cliente para enviar inputs al servidor
    Car& player_car; // referencia al auto local (para animaciones)

public:
    EventHandler(Client& client, Car& player_car);

    // procesa todos los eventos SDL y los reenvía al cliente
    bool handle_events();
};

#endif // EVENT_HANDLER_H