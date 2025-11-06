#ifndef QUEUE_H_
#define QUEUE_H_

#include <climits>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <queue>
#include <stdexcept>

struct ClosedQueue: public std::runtime_error {
    ClosedQueue(): std::runtime_error("The queue is closed") {}
};

/*
 * Multiproducer/Multiconsumer Blocking Queue (MPMC)
 *
 * Queue is a generic MPMC queue with blocking operations
 * push() and pop().
 *
 * Two additional methods, tryPush() and tryPop() allow
 * non-blocking operations.
 *
 * On a closed queue, any method will raise ClosedQueue.
 *
 * */
template <typename T, class C = std::deque<T> >
class Queue {
private:
    std::queue<T, C> q;
    const unsigned int maxSize;

    bool closed;

    std::mutex mtx;
    std::condition_variable isNotFull;
    std::condition_variable isNotEmpty;

public:
    Queue(): maxSize(UINT_MAX - 1), closed(false) {}
    explicit Queue(const unsigned int maxSize): maxSize(maxSize), closed(false) {}


    bool tryPush(T const& val) {
        std::unique_lock<std::mutex> lck(mtx);

        if (closed) {
            throw ClosedQueue();
        }

        if (q.size() == this->maxSize) {
            return false;
        }

        if (q.empty()) {
            isNotEmpty.notify_all();
        }

        q.push(val);
        return true;
    }

    bool tryPop(T& val) {
        std::unique_lock<std::mutex> lck(mtx);

        if (q.empty()) {
            if (closed) {
                throw ClosedQueue();
            }
            return false;
        }

        if (q.size() == this->maxSize) {
            isNotFull.notify_all();
        }

        val = q.front();
        q.pop();
        return true;
    }

    void push(T const& val) {
        std::unique_lock<std::mutex> lck(mtx);

        if (closed) {
            throw ClosedQueue();
        }

        while (q.size() == this->maxSize) {
            isNotFull.wait(lck);
        }

        if (q.empty()) {
            isNotEmpty.notify_all();
        }

        q.push(val);
    }

    // cppcheck-suppress duplInheritedMember
    T pop() {
        std::unique_lock<std::mutex> lck(mtx);

        while (q.empty()) {
            if (closed) {
                throw ClosedQueue();
            }
            isNotEmpty.wait(lck);
        }

        if (q.size() == this->maxSize) {
            isNotFull.notify_all();
        }

        T const val = q.front();
        q.pop();

        return val;
    }

    // cppcheck-suppress duplInheritedMember
    void close() {
        std::unique_lock<std::mutex> lck(mtx);

        if (closed) {
            throw std::runtime_error("The queue is already closed.");
        }

        closed = true;
        isNotEmpty.notify_all();
    }

private:
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
};

template <>
class Queue<void*> {
private:
    std::queue<void*> q;
    const unsigned int maxSize;

    bool closed;

    std::mutex mtx;
    std::condition_variable isNotFull;
    std::condition_variable isNotEmpty;

public:
    explicit Queue(const unsigned int maxSize): maxSize(maxSize), closed(false) {}


    bool tryPush(void* const& val) {
        std::unique_lock<std::mutex> lck(mtx);

        if (closed) {
            throw ClosedQueue();
        }

        if (q.size() == this->maxSize) {
            return false;
        }

        if (q.empty()) {
            isNotEmpty.notify_all();
        }

        q.push(val);
        return true;
    }

    bool tryPop(void*& val) {
        std::unique_lock<std::mutex> lck(mtx);

        if (q.empty()) {
            if (closed) {
                throw ClosedQueue();
            }
            return false;
        }

        if (q.size() == this->maxSize) {
            isNotFull.notify_all();
        }

        val = q.front();
        q.pop();
        return true;
    }

    void push(void* const& val) {
        std::unique_lock<std::mutex> lck(mtx);

        if (closed) {
            throw ClosedQueue();
        }

        while (q.size() == this->maxSize) {
            isNotFull.wait(lck);
        }

        if (q.empty()) {
            isNotEmpty.notify_all();
        }

        q.push(val);
    }


    void* pop() {
        std::unique_lock<std::mutex> lck(mtx);

        while (q.empty()) {
            if (closed) {
                throw ClosedQueue();
            }
            isNotEmpty.wait(lck);
        }

        if (q.size() == this->maxSize) {
            isNotFull.notify_all();
        }

        void* const val = q.front();
        q.pop();

        return val;
    }

    void close() {
        std::unique_lock<std::mutex> lck(mtx);

        if (closed) {
            throw std::runtime_error("The queue is already closed.");
        }

        closed = true;
        isNotEmpty.notify_all();
    }

private:
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
};


template <typename T>
class Queue<T*>: private Queue<void*> {
public:
    explicit Queue(const unsigned int maxSize): Queue<void*>(maxSize) {}


    bool tryPush(T* const& val) { return Queue<void*>::tryPush(val); }

    bool tryPop(T*& val) { return Queue<void*>::tryPop(static_cast<void*&>(val)); }

    void push(T* const& val) { return Queue<void*>::push(val); }

    // cppcheck-suppress duplInheritedMember
    T* pop() { return (T*)Queue<void*>::pop(); }

    // cppcheck-suppress duplInheritedMember
    void close() { return Queue<void*>::close(); }

private:
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
};

#endif  // QUEUE_H_
