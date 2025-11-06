#ifndef THREAD_H_
#define THREAD_H_

#include <atomic>
#include <thread>

#include "runnable.h"

class Thread: public Runnable {
private:
    std::thread thread;

    /**
     * Subclasses that inherit from Thread will have access to these
     * flags, mostly to control how Thread::run() will behave
     */
    std::atomic<bool> _keepRunning;
    std::atomic<bool> _isAlive;

protected:
    /**
     * Returns true if the thread should keep running, false otherwise
     */
    bool shouldKeepRunning() const;

public:
    /**
     * Constructor: initializes the flags
     */
    Thread();

    /**
     * Creates/starts the thread, which will execute Thread::main()
     */
    void start() override;

    /**
     * Joins the thread (waits for it to finish)
     */
    void join() override;

    /**
     * This is the function that is actually executed in the new thread
     * It calls the subclass's run() method and catches/logs any exception
     * that may escape from it, setting _isAlive = false before returning.
     */
    void main();

    /**
     * Note: it is up to the subclass to make something meaningful to
     * really stop the thread. The Thread::run() may be blocked and/or
     * it may not read _keepRunning.
     */
    void stop() override;

    /**
     * Note: asking for isAlive is well defined *only if* the thread
     * was started (you called Thread::start())
     */
    bool isAlive() const override;

    /**
     * The subclass must implement
     */
    virtual void run() = 0;

    /**
     * Destructor
     */
    virtual ~Thread() {}

    /**
     * Disable copy and move semantics (not needed and error-prone)
     */
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    Thread(Thread&& other) = delete;
    Thread& operator=(Thread&& other) = delete;
};

#endif  // THREAD_H_
