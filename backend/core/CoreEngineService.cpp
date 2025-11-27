#include "CoreEngineService.h"
CoreEngineService::CoreEngineService()
    : graph(0),
      emergency(),
      timeSeries(1440) // default: 1440 minutes (1 day)
{}

void CoreEngineService::loadCityGraph(const std::string &nodes,
                                      const std::string &edges) {
    graph.loadGraph(nodes, edges);
}

void CoreEngineService::addRoad(int u, int v, double weight, int id) {
    graph.addEdge(u, v, weight, id);
}

void CoreEngineService::reserveNodes(int n) {
    graph.reserveNodes(n);
}

std::vector<double> CoreEngineService::computeRoute(int src) {
    return graph.dijkstra(src);
}

void CoreEngineService::addEmergencyRequest(int id,
                                            int sourceNode,
                                            const std::string &type,
                                            double priority) {
    emergency.addEmergency(id, sourceNode, type, priority);
}

bool CoreEngineService::hasPendingEmergency() const {
    return emergency.hasEmergency();
}

int CoreEngineService::processNextEmergency(std::vector<double> &routeOut) {
    if (!emergency.hasEmergency()) return -1;
    EmergencyRequest req = emergency.getNextEmergency();
    // For demo: compute route from sourceNode
    routeOut = graph.dijkstra(req.sourceNode);
    return req.id;
}

void CoreEngineService::updateTraffic(int timeSlot, int delta) {
    timeSeries.pointUpdate(timeSlot, delta);
}

int CoreEngineService::getTrafficRange(int start, int end) const {
    return timeSeries.rangeQuery(start, end);
}

void CoreEngineService::applyCongestionToEdge(int u, int v, double multiplier) {
    graph.setCongestion(u, v, multiplier);
}

std::vector<int> CoreEngineService::computePath(int src, int dest) {
    return graph.shortestPath(src, dest);
}
