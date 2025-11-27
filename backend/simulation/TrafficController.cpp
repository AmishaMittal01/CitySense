// ===================== TrafficController.cpp =====================
#include "TrafficController.h"
#include <iostream>

TrafficController::TrafficController() {}

// ---------------- SETUP ----------------

void TrafficController::addIntersection(int intersectionID) {
    if (intersectionQueues.find(intersectionID) == intersectionQueues.end()) {
        intersectionQueues[intersectionID] = {};
        signalPhases[intersectionID] = {};
        currentPhaseIndex[intersectionID] = 0;
    }
}

void TrafficController::addSignalPhase(int intersectionID,
                                       const std::string &direction,
                                       int duration) {
    addIntersection(intersectionID);
    signalPhases[intersectionID].push_back({direction, duration});
}

// ---------------- QUEUE HANDLING ----------------

void TrafficController::enqueueVehicle(int intersectionID,
                                       const std::string &direction,
                                       int vehicleID) {
    addIntersection(intersectionID);
    intersectionQueues[intersectionID][direction].vehicles.push(vehicleID);
}

int TrafficController::dequeueVehicle(int intersectionID) {
    std::string currentDir = getCurrentDirection(intersectionID);
    if (currentDir.empty()) return -1;

    auto &lane = intersectionQueues[intersectionID][currentDir].vehicles;

    if (lane.empty()) return -1;
    int v = lane.front();
    lane.pop();
    return v;
}

// ---------------- SIGNAL HANDLING ----------------

void TrafficController::advancePhase(int intersectionID) {
    auto it = signalPhases.find(intersectionID);
    if (it == signalPhases.end() || it->second.empty()) return;

    int &idx = currentPhaseIndex[intersectionID];
    idx = (idx + 1) % static_cast<int>(it->second.size());
}

std::string TrafficController::getCurrentDirection(int intersectionID) const {
    auto it = signalPhases.find(intersectionID);
    if (it == signalPhases.end() || it->second.empty()) return "";

    auto idxIt = currentPhaseIndex.find(intersectionID);
    int idx = 0;
    if (idxIt != currentPhaseIndex.end()) idx = idxIt->second;
    if (idx < 0 || idx >= static_cast<int>(it->second.size())) idx = 0;

    return it->second[idx].direction;
}

// ---------------- MONITORING ----------------

int TrafficController::getQueueLength(int intersectionID,
                                      const std::string &direction) const {
    auto it = intersectionQueues.find(intersectionID);
    if (it == intersectionQueues.end()) return 0;

    auto laneIt = it->second.find(direction);
    if (laneIt == it->second.end()) return 0;

    return static_cast<int>(laneIt->second.vehicles.size());
}
