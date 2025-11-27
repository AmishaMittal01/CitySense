// ===================== TrafficController.h =====================
#ifndef TRAFFIC_CONTROLLER_H
#define TRAFFIC_CONTROLLER_H

#include <queue>
#include <unordered_map>
#include <vector>
#include <string>

// Represents a lane queue at an intersection
struct LaneQueue {
    std::queue<int> vehicles;   // store vehicle IDs
};

// Represents a traffic light phase
struct SignalPhase {
    std::string direction; // e.g. "N-S", "E-W"
    int duration;          // in seconds (for future extension)
};

class TrafficController {
private:
    // intersectionID -> direction -> queue
    std::unordered_map<int, std::unordered_map<std::string, LaneQueue>> intersectionQueues;

    // intersectionID -> list of phases
    std::unordered_map<int, std::vector<SignalPhase>> signalPhases;

    // intersectionID -> current phase index
    std::unordered_map<int, int> currentPhaseIndex;

public:
    TrafficController();

    // ---------------- SETUP ----------------
    void addIntersection(int intersectionID);
    void addSignalPhase(int intersectionID, const std::string &direction, int duration);

    // ---------------- QUEUE HANDLING ----------------
    void enqueueVehicle(int intersectionID, const std::string &direction, int vehicleID);
    int dequeueVehicle(int intersectionID);

    // ---------------- SIGNAL HANDLING ----------------
    void advancePhase(int intersectionID);
    std::string getCurrentDirection(int intersectionID) const;

    // ---------------- MONITORING ----------------
    int getQueueLength(int intersectionID, const std::string &direction) const;
};

#endif
