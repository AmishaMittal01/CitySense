// ===================== GraphManager.cpp =====================
#include "GraphManager.h"
#include <queue>
#include <fstream>
#include <sstream>
#include <limits>
#include <iostream>
#include <algorithm>

GraphManager::GraphManager(int nodes) {
    reserveNodes(nodes);
}

void GraphManager::reserveNodes(int nodes) {
    n = std::max(0, nodes);
    adj.assign(n + 1, {});
    adjMatrix.assign(n + 1, std::vector<double>(n + 1,
                      std::numeric_limits<double>::infinity()));
    for (int i = 1; i <= n; ++i) {
        adjMatrix[i][i] = 0.0;
    }
}

void GraphManager::addEdge(int u, int v, double w, int id) {
    if (u <= 0 || v <= 0) return;
    int maxNode = std::max(u, v);
    if (maxNode > n) {
        // Expand graph if needed
        reserveNodes(maxNode);
    }

    adj[u].push_back({v, w, id});

    if (u >= 0 && u < (int)adjMatrix.size() &&
        v >= 0 && v < (int)adjMatrix.size()) {
        adjMatrix[u][v] = std::min(adjMatrix[u][v], w);
    }
}

void GraphManager::loadGraph(const std::string &nodeFile, const std::string &edgeFile) {
    // Determine number of nodes from node file (assuming each line has an ID)
    std::ifstream nf(nodeFile);
    int maxId = 0;
    if (nf) {
        std::string line;
        while (std::getline(nf, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            int id;
            if (iss >> id) {
                maxId = std::max(maxId, id);
            }
        }
    }

    if (maxId > 0) {
        reserveNodes(maxId);
    }

    std::ifstream ef(edgeFile);
    if (!ef) {
        std::cerr << "Failed to open edge file: " << edgeFile << "\n";
        return;
    }

    std::string line;
    while (std::getline(ef, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);

        int u, v, id = 0;
        double w;
        if (!(iss >> u >> v >> w)) continue;
        if (!(iss >> id)) id = 0;

        addEdge(u, v, w, id);
    }
}

void GraphManager::setCongestion(int u, int v, double mult) {
    if (mult <= 0.0) mult = 1.0;
    congestionMultiplier[key(u, v)] = mult;
}

double GraphManager::getCongestion(int u, int v) const {
    auto it = congestionMultiplier.find(key(u, v));
    if (it == congestionMultiplier.end()) return 1.0;
    return it->second;
}

std::vector<double> GraphManager::dijkstra(int src) const {
    const double INF = std::numeric_limits<double>::infinity();
    std::vector<double> dist(n + 1, INF);

    if (src < 1 || src > n) return dist;

    using State = std::pair<double, int>; // (dist, node)
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

    dist[src] = 0.0;
    pq.push({0.0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;

        for (const auto &e : adj[u]) {
            double wt = e.weight * getCongestion(u, e.to);
            if (dist[e.to] > d + wt) {
                dist[e.to] = d + wt;
                pq.push({dist[e.to], e.to});
            }
        }
    }

    return dist;
}
std::vector<int> GraphManager::shortestPath(int src, int dest) {
    const double INF = std::numeric_limits<double>::infinity();
    std::vector<double> dist(n + 1, INF);
    std::vector<int> parent(n + 1, -1);

    using State = std::pair<double, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

    dist[src] = 0;
    pq.push({0, src});
    parent[src] = src;

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (u == dest) break;
        if (d > dist[u]) continue;

        for (const auto &e : adj[u]) {
            double wt = e.weight * getCongestion(u, e.to);
            if (dist[e.to] > d + wt) {
                dist[e.to] = d + wt;
                parent[e.to] = u;
                pq.push({dist[e.to], e.to});
            }
        }
    }

    if (dist[dest] == INF) return {};

    std::vector<int> path;
    for (int v = dest; v != src; v = parent[v]) {
        if (v == -1) return {};
        path.push_back(v);
    }
    path.push_back(src);
    std::reverse(path.begin(), path.end());
    return path;
}
