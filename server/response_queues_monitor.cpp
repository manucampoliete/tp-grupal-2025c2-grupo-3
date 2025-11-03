
#include "response_queues_monitor.h"

ResponseQueuesMonitor::ResponseQueuesMonitor() {}

Queue<Snapshot>& ResponseQueuesMonitor::add_queue(ClientID client_id) {
    std::lock_guard<std::mutex> lock(mtx);
    return response_queues[client_id];  // What about std::map::insert or std::map::emplace?
}

std::size_t ResponseQueuesMonitor::remove_queue(ClientID client_id) {
    std::lock_guard<std::mutex> lock(mtx);
    return response_queues.erase(client_id);
}

bool ResponseQueuesMonitor::broadcast(const Snapshot& resp) {
    std::lock_guard<std::mutex> lock(mtx);
    bool all = true;
    for (auto& pair: response_queues) {
        all &= pair.second.try_push(resp);  // We do not want to block the gameloop
    }
    return all;
}

void ResponseQueuesMonitor::close_all() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& pair: response_queues) {
        pair.second.close();
    }
}

ResponseQueuesMonitor::~ResponseQueuesMonitor() {}
