// ===================== ParkingManager.h =====================
#ifndef PARKING_MANAGER_H
#define PARKING_MANAGER_H

#include <unordered_map>
#include <string>
#include <vector>
#include <stack>

// Represents a single parking spot
struct ParkingSpot {
    std::string spotID;
    int zone;
    bool occupied;
    int vehicleID;
};

struct ParkingUndoAction {
    std::string spotID;
    bool prevOccupied;
    int prevVehicleID;
};

class ParkingManager {
private:
    // spotID -> ParkingSpot
    std::unordered_map<std::string, ParkingSpot> spots;

    // zone -> list of spotIDs
    std::unordered_map<int, std::vector<std::string>> zoneMap;

    // stack for undo operations
    std::stack<ParkingUndoAction> undoStack;

public:
    ParkingManager() = default;

    // ----------- SETUP -----------
    void addSpot(const std::string &spotID, int zone);
    void loadFromFile(const std::string &filename);  // optional CSV / text

    // ----------- ALLOCATION -----------
    bool assignSpot(const std::string &spotID, int vehicleID);
    std::string findNearestFreeSpot(int zone) const;

    // ----------- UNDO FEATURE -----------
    void undoLastChange();

    // ----------- STATUS -----------
    bool isOccupied(const std::string &spotID) const;
    int getVehicleInSpot(const std::string &spotID) const;
    std::vector<std::string> getFreeSpotsInZone(int zone) const;
};

#endif
