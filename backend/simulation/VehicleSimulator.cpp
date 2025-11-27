// ===================== VehicleSimulator.cpp =====================
#include "VehicleSimulator.h"
#include <iostream>
#include <limits>

VehicleSimulator::VehicleSimulator(TrafficController *t,
                                   ParkingManager *p,
                                   CoreEngineService *c)
    : trafficController(t), parkingManager(p), coreEngine(c) {}

// ---------------- VEHICLE MANAGEMENT ----------------

void VehicleSimulator::addVehicle(int id, int startNode, int destinationNode) {
    Vehicle v;
    v.id = id;
    v.currentNode = startNode;
    v.destinationNode = destinationNode;
    v.parked = false;
    vehicles[id] = v;
}

void VehicleSimulator::removeVehicle(int id) {
    vehicles.erase(id);
}

// ---------------- MOVEMENT / SIMULATION ----------------

void VehicleSimulator::moveVehicle(int id) {
    auto it = vehicles.find(id);
    if (it == vehicles.end() || !coreEngine) return;

    Vehicle &v = it->second;
    if (v.parked) return;

    // For now, just compute distance and "jump" to destination if reachable.
    std::vector<double> dist = coreEngine->computeRoute(v.currentNode);

    if (v.destinationNode <= 0 ||
        v.destinationNode >= static_cast<int>(dist.size())) {
        return;
    }

    if (dist[v.destinationNode] == std::numeric_limits<double>::infinity()) {
        // No path known
        return;
    }

    // Save undo action
    UndoAction action;
    action.type = "movement";
    action.targetID = std::to_string(v.id);
    action.prevOccupied = false;
    action.prevVehicleID = v.currentNode;
    undoStack.pushAction(action);

    // Simplified: directly move to destination
    v.currentNode = v.destinationNode;
}

void VehicleSimulator::simulateStep() {
    for (auto &kv : vehicles) {
        moveVehicle(kv.first);
    }
}

// ---------------- PARKING ----------------

bool VehicleSimulator::tryParking(int vehicleID, int zone) {
    if (!parkingManager) return false;

    auto it = vehicles.find(vehicleID);
    if (it == vehicles.end()) return false;

    Vehicle &v = it->second;
    if (v.parked) return false;

    std::string spotID = parkingManager->findNearestFreeSpot(zone);
    if (spotID.empty()) return false;

    if (!parkingManager->assignSpot(spotID, vehicleID)) return false;

    // Save parking undo action (we delegate actual state revert to ParkingManager)
    UndoAction action;
    action.type = "parking";
    action.targetID = spotID;
    action.prevOccupied = false;
    action.prevVehicleID = -1;
    undoStack.pushAction(action);

    v.parked = true;
    return true;
}

// ---------------- UNDO ----------------

void VehicleSimulator::undoLastAction() {
    if (!undoStack.hasUndo()) return;

    UndoAction a = undoStack.popAction();

    if (a.type == "movement") {
        int id = std::stoi(a.targetID);
        auto it = vehicles.find(id);
        if (it != vehicles.end()) {
            it->second.currentNode = a.prevVehicleID;
        }
    } else if (a.type == "parking") {
        if (parkingManager) {
            parkingManager->undoLastChange();
        }
    }
}
