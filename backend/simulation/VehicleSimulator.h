// ===================== VehicleSimulator.h =====================
#ifndef VEHICLE_SIMULATOR_H
#define VEHICLE_SIMULATOR_H

#include <unordered_map>
#include <vector>
#include <string>
#include "TrafficController.h"
#include "ParkingManager.h"
#include "UndoStack.h"
#include "../core/CoreEngineService.h"   // Person 1 integration

// Represents a vehicle in the city simulation
struct Vehicle {
    int id;
    int currentNode;
    int destinationNode;
    bool parked;
};

class VehicleSimulator {
private:
    std::unordered_map<int, Vehicle> vehicles; // vehicleID -> Vehicle

    TrafficController *trafficController;
    ParkingManager *parkingManager;
    CoreEngineService *coreEngine;
    UndoStack undoStack;

public:
    VehicleSimulator(TrafficController *t, ParkingManager *p, CoreEngineService *c);

    // ---------------- VEHICLE MANAGEMENT ----------------
    void addVehicle(int id, int startNode, int destinationNode);
    void removeVehicle(int id);

    // ---------------- MOVEMENT / SIMULATION ----------------
    void moveVehicle(int id);
    void simulateStep();

    // ---------------- PARKING ----------------
    bool tryParking(int vehicleID, int zone);

    // ---------------- UNDO ----------------
    void undoLastAction();
};

#endif
