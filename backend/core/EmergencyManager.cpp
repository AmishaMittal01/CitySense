#include "EmergencyManager.h"
#include <stdexcept>
void EmergencyManager::addEmergency(int id,
                                    int sourceNode,
                                    const std::string &type,
                                    double priority) {
    EmergencyRequest req;
    req.id = id;
    req.sourceNode = sourceNode;
    req.type = type;
    req.priority = priority;

    pq.push(req);
}
bool EmergencyManager::hasEmergency() const {
    return !pq.empty();
}

EmergencyRequest EmergencyManager::peekEmergency() const {
    if (pq.empty()) throw std::runtime_error("No emergencies to peek.");
    return pq.top();
}

EmergencyRequest EmergencyManager::getNextEmergency() {
    if (pq.empty()) throw std::runtime_error("No emergencies available.");
    EmergencyRequest top = pq.top();
    pq.pop();
    return top;
}
