#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <cstdint>
#include <vector>
#include <string>

#include "../common/types/types.h"  // For CarID

struct CarInfo {  /** NOTE: We should merge this with CarInfo in "../common/utils/carinfo.h" */
    const CarID id;
    const float maxSpeed;
    const float acceleration;
    const float angularSpeed;
    const float mass;
    const std::string name;
    const float health;

    CarInfo(CarID id, 
            float maxSpeed, 
            float acceleration,
            float angularSpeed,
            float mass,
            std::string name,
            float health)
        : id(id),
          maxSpeed(maxSpeed),
          acceleration(acceleration),
          angularSpeed(angularSpeed),
          mass(mass),
          name(name),
          health(health) {}
};

struct GamePhasesTimers {
    const uint8_t countdown;      // Seconds
    const uint8_t racing;         // Minutes
    const uint8_t showingStats;   // Seconds
    const uint8_t modifyingCar;   // Seconds
    const uint8_t gameEnd;        // Seconds

    GamePhasesTimers(uint8_t countdown,
                     uint8_t racing,
                     uint8_t showingStats,
                     uint8_t modifyingCar,
                     uint8_t gameEnd)
        : countdown(countdown),
          racing(racing),
          showingStats(showingStats),
          modifyingCar(modifyingCar),
          gameEnd(gameEnd) {}
};

struct Config {
    const std::vector<CarInfo> carsInfo;
    const GamePhasesTimers gamePhasesTimers;
    const uint8_t numberOfRaces;
    const uint8_t rateLoop;

    Config(std::vector<CarInfo>&& carsInfo,
           const GamePhasesTimers& gamePhasesTimers,
           uint8_t numberOfRaces,
           uint8_t rateLoop)
        : carsInfo(std::move(carsInfo)),
          gamePhasesTimers(gamePhasesTimers),
          numberOfRaces(numberOfRaces),
          rateLoop(rateLoop) {}
};

class ConfigLoader {
public:
    /**
     * Loads the configuration from a YAML file.
     */
    static Config Load(const std::string& configYamlPath);
};

#endif  // CONFIG_LOADER_H
