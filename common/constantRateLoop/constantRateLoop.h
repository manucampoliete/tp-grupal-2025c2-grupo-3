#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <thread>

/**
 * OPCION 1
 */

class ConstantRateLoop {
public:
    ConstantRateLoop() {}

    void run(const std::function<bool(uint64_t)>& func, double rate = (1000.0 / 30)) {
        double t1 = now();
        uint64_t it = 0;
        while (true) {
            if (!func(it))
                break;

            double t2 = now();
            double rest = rate - (t2 - t1);
            if (rest < 0) {
                double behind = -rest;
                rest = rate - std::fmod(behind, rate);
                double lost = behind + rest;
                t1 += lost;
                it += static_cast<uint64_t>(lost / rate);
            }

            sleep(rest);
            t1 += rate;
            it++;
        }
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
//     ConstantRateLoop().run(foo);
//     return 0;
// }
