#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <thread>

/**
 * OPCION 2
 */

#define FPS 60
#define RATE (1000.0 / FPS)

class ConstantRateLoop {
private:
    double t1;
    uint64_t it;

public:
    ConstantRateLoop(): t1(now()), it(0) {}

    uint64_t sleepAndCalcIt() {
        double t2 = now();
        double rest = RATE - (t2 - t1);

        if (rest < 0) {
            double behind = -rest;
            rest = RATE - std::fmod(behind, RATE);
            double lost = behind + rest;
            t1 += lost;
            it += static_cast<uint64_t>(lost / RATE);
        }

        sleep(rest);
        t1 += RATE;
        return ++it;
    }

    double now() {
        using namespace std::chrono;
        return duration_cast<duration<double, std::milli>>(steady_clock::now().time_since_epoch())
                .count();
    }

    void sleep(double ms) {
        if (ms <= 0.0)
            return;
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(ms));
    }
};

// #include <iostream>

// bool foo(uint64_t it) {
//     std::cout << "Iteration " << it << std::endl;
//     return it < 300;
// }

// int main() {
//     uint64_t it = 0;
//     ConstantRateLoop constantRateLoop;
//     while (true) {
//         if (!foo(it)) break;
//         it = constantRateLoop.sleepAndCalcIt();
//     }
//     return 0;
// }
