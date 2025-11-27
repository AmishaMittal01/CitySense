#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H
#include <vector>
#include <string>
#include <unordered_map>
struct Edge {
    int to;
    double weight; 
    int id;       
};
class GraphManager {
private:
    int n; 
    std::vector<std::vector<Edge>> adj;
    std::vector<std::vector<double>> adjMatrix;
    std::unordered_map<long long, double> congestionMultiplier;

    long long key(int u, int v) const {
        return (static_cast<long long>(u) << 32) |
               static_cast<unsigned long long>(v);
    }

public:
    GraphManager(int nodes = 0);
    void reserveNodes(int nodes);
    void addEdge(int u, int v, double w, int id = 0);
    void loadGraph(const std::string &nodeFile, const std::string &edgeFile);
    void setCongestion(int u, int v, double mult);
    double getCongestion(int u, int v) const;
    std::vector<double> dijkstra(int src) const;
    std::vector<int> shortestPath(int src, int dest);
};
#endif
