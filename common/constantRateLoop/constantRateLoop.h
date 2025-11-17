#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <thread>

#define FPS 60
#define RATE (1000.0 / FPS)

class ConstantRateLoop {
private:
    double t1;
    uint64_t it;

public:
    /**
     * Constructor
     */
    ConstantRateLoop();

    /**
     * Sleeps the necessary time to maintain a constant rate loop and calculates the current iteration.
     */
    uint64_t sleepAndCalcIt();

    /**
     * Gets the current time in milliseconds.
     */
    double now();

    /**
     * Sleeps for the specified number of milliseconds.
     */
    void sleep(double ms);
};

/**
 * How to use it:
 */

// #include <iostream>
//
// bool foo(uint64_t it) {
//     std::cout << "Iteration " << it << std::endl;
//     return it < 300;
// }
//
// int main() {
//     uint64_t it = 0;
//     ConstantRateLoop constantRateLoop;
//     while (true) {
//         if (!foo(it)) break;
//         it = constantRateLoop.sleepAndCalcIt();
//     }
//     return 0;
// }
