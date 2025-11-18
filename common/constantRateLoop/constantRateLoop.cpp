#include "constantRateLoop.h"

ConstantRateLoop::ConstantRateLoop(): t1(now()), it(0) {}

uint64_t ConstantRateLoop::sleepAndCalcIt() {
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

double ConstantRateLoop::now() {
    using namespace std::chrono;
    return duration_cast<duration<double, std::milli>>(steady_clock::now().time_since_epoch())
            .count();
}

void ConstantRateLoop::sleep(double ms) {
    if (ms <= 0.0)
        return;
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(ms));
}
