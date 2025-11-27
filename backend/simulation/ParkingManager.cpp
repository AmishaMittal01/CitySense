// ===================== ParkingManager.cpp =====================
#include "ParkingManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

// ---------------------- SETUP ----------------------

void ParkingManager::addSpot(const std::string &spotID, int zone) {
    ParkingSpot spot;
    spot.spotID = spotID;
    spot.zone = zone;
    spot.occupied = false;
    spot.vehicleID = -1;

    spots[spotID] = spot;
    zoneMap[zone].push_back(spotID);
}

void ParkingManager::loadFromFile(const std::string &filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Failed to open parking file: " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id;
        int zone = 0;

        if (line.find(',') != std::string::npos) {
            std::string token;
            std::vector<std::string> row;
            while (std::getline(ss, token, ',')) row.push_back(token);
            if (row.size() < 2) continue;
            id = row[0];
            zone = std::stoi(row[1]);
        } else {
            ss >> id >> zone;
        }

        addSpot(id, zone);
    }
}

// ---------------------- ALLOCATION ----------------------

bool ParkingManager::assignSpot(const std::string &spotID, int vehicleID) {
    auto it = spots.find(spotID);
    if (it == spots.end()) return false;

    ParkingSpot &spot = it->second;
    if (spot.occupied) return false;

    ParkingUndoAction action;
    action.spotID = spotID;
    action.prevOccupied = spot.occupied;
    action.prevVehicleID = spot.vehicleID;
    undoStack.push(action);

    spot.occupied = true;
    spot.vehicleID = vehicleID;
    return true;
}

std::string ParkingManager::findNearestFreeSpot(int zone) const {
    auto it = zoneMap.find(zone);
    if (it == zoneMap.end()) return "";

    for (const auto &spotID : it->second) {
        auto sit = spots.find(spotID);
        if (sit != spots.end() && !sit->second.occupied) {
            return spotID;
        }
    }
    return "";
}

// ---------------------- UNDO FEATURE ----------------------

void ParkingManager::undoLastChange() {
    if (undoStack.empty()) return;

    ParkingUndoAction action = undoStack.top();
    undoStack.pop();

    auto it = spots.find(action.spotID);
    if (it == spots.end()) return;

    it->second.occupied = action.prevOccupied;
    it->second.vehicleID = action.prevVehicleID;
}

// ---------------------- STATUS ----------------------

bool ParkingManager::isOccupied(const std::string &spotID) const {
    auto it = spots.find(spotID);
    if (it == spots.end()) return false;
    return it->second.occupied;
}

int ParkingManager::getVehicleInSpot(const std::string &spotID) const {
    auto it = spots.find(spotID);
    if (it == spots.end()) return -1;
    return it->second.vehicleID;
}

std::vector<std::string> ParkingManager::getFreeSpotsInZone(int zone) const {
    std::vector<std::string> freeSpots;
    auto it = zoneMap.find(zone);
    if (it == zoneMap.end()) return freeSpots;

    for (auto &spotID : it->second) {
        auto sit = spots.find(spotID);
        if (sit != spots.end() && !sit->second.occupied)
            freeSpots.push_back(spotID);
    }

    return freeSpots;
}
