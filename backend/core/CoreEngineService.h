#ifndef CORE_ENGINE_SERVICE_H
#define CORE_ENGINE_SERVICE_H

#include "GraphManager.h"
#include "EmergencyManager.h"
#include "TimeSeriesManager.h"
#include <vector>
#include <string>

class CoreEngineService {
private:
    GraphManager graph;
    EmergencyManager emergency;
    TimeSeriesManager timeSeries;

public:
    CoreEngineService();
    void reserveNodes(int n);
    std::vector<int> computePath(int src, int dest);

    void loadCityGraph(const std::string &nodesFile, const std::string &edgesFile);
    void addRoad(int u, int v, double weight, int id = 0);
    std::vector<double> computeRoute(int src);

    void addEmergencyRequest(int id, int sourceNode, const std::string &type, double priority);
    bool hasPendingEmergency() const;
    int processNextEmergency(std::vector<double> &routeOut);

    void updateTraffic(int timeSlot, int delta);
    int getTrafficRange(int start, int end) const;
    void applyCongestionToEdge(int u, int v, double multiplier);
};
#endif
