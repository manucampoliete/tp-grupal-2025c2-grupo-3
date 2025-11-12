#include "world.h"


// para evitar que el hilo Receiver (que escribe los datos) y el hilo Game (que los lee para
// dibujar) accedan a la misma memoria al mismo tiempo


// limpia el estado del mundo local
// se usa para una nueva carrera o al reiniciar el juego
void World::reset() {
    std::lock_guard<std::mutex> lock(mtx);
    cars.clear();
    countdown = 0;
}


// actualiza la copia local del mundo con un nuevo broadcast del server
// cada auto se actualiza según su id
// llamado por el Receiver
void World::update(const BroadcastData& data) {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& car: data.cars) {
        cars[car.id] = car;  // actualiza el estado de ese auto
    }
    countdown = data.countdown;
}


// devuelve una copia del estado de todos los autos
// se usa desde el hilo del Game (render)
std::map<uint8_t, BroadcastData::CarState> World::getCars() const {
    std::lock_guard<std::mutex> lock(mtx);
    return cars;
}


// devuelve el tiempo restante del contador de carrera
uint32_t World::getCountdown() const {
    std::lock_guard<std::mutex> lock(mtx);
    return countdown;
}
