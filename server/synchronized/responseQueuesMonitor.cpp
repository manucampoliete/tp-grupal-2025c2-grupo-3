
#include "responseQueuesMonitor.h"

ResponseQueuesMonitor::ResponseQueuesMonitor() {}

Queue<std::shared_ptr<Snapshot>>& ResponseQueuesMonitor::addQueue(ClientID clientId) {
    std::lock_guard<std::mutex> lock(mtx);
    return responseQueues[clientId];  // std::map::insert? std::map::emplace?
}

Queue<std::shared_ptr<Snapshot>>& ResponseQueuesMonitor::getQueue(ClientID clientId) {
    std::lock_guard<std::mutex> lock(mtx);
    return responseQueues.at(clientId);
}

std::size_t ResponseQueuesMonitor::removeQueue(ClientID clientId) {
    std::lock_guard<std::mutex> lock(mtx);
    return responseQueues.erase(clientId);
}

bool ResponseQueuesMonitor::broadcast(std::shared_ptr<Snapshot> snapshot) {
    std::lock_guard<std::mutex> lock(mtx);
    bool all = true;
    for (auto& pair: responseQueues) {
        all &= pair.second.tryPush(snapshot);  // We do not want to block the gameloop
    }
    return all;
}

void ResponseQueuesMonitor::closeAll() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& pair: responseQueues) {
        pair.second.close();
    }
}

ResponseQueuesMonitor::~ResponseQueuesMonitor() {}
