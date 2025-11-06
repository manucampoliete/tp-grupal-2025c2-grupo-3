#include "thread.h"

#include <iostream>

#include <syslog.h>

bool Thread::shouldKeepRunning() const { return _keepRunning; }

Thread::Thread(): _keepRunning(true), _isAlive(false) {}

void Thread::start() {
    _isAlive = true;
    _keepRunning = true;
    thread = std::thread(&Thread::main, this);
}

void Thread::join() { thread.join(); }

void Thread::main() {
    try {
        this->run();
    } catch (const std::exception& err) {
        syslog(LOG_CRIT, "[Crit] Error!: %s", err.what());
    } catch (...) {
        syslog(LOG_CRIT, "[Crit] Unknown error!");
    }

    _isAlive = false;
}

void Thread::stop() { _keepRunning = false; }

bool Thread::isAlive() const { return _isAlive; }
