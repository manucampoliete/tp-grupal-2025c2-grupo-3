#ifndef WORLD_H
#define WORLD_H

#include <cstdint>
#include <map>
#include <mutex>
#include <vector>

#include "../common/messages/game_data.h"


// mantiene el estado del mundo local de cada cliente
// cada cliente tiene su propio World que refleja lo que el servidor le envía en los broadcasts

class World {
private:
    std::map<uint8_t, BroadcastData::CarState> cars; // estado de cada auto
    uint32_t countdown = 0; // cuenta regresiva
    mutable std::mutex mtx; // para poder bloquear en métodos const

public:
    // limpia el estado del mundo (por ejemplo, al iniciar una nueva carrera)
    void reset();

    // actualiza el estado con un nuevo broadcast recibido del servidor
    void update(const BroadcastData& data);

    // obtiene una copia del estado de todos los autos
    std::map<uint8_t, BroadcastData::CarState> getCars() const;

    // obtiene el tiempo restante
    uint32_t getCountdown() const;
};

#endif  // WORLD_H
