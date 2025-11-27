#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include "core/CoreEngineService.h"

static CoreEngineService engine;

void initEngine() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    std::cerr << "Engine initialized for JIIT + Indirapuram graph\n";
    engine.reserveNodes(12);

    // Node IDs:
    //  1 - Noida Sector 62 Metro
    //  2 - Fortis Hospital
    //  3 - The Corenthum
    //  4 - Peace Softtech (C-56/21)
    //  5 - Logix Cyber Park
    //  6 - Stellar IT Park
    //  7 - JIIT Sector 62
    //  8 - Shipra Mall (Indirapuram)
    //  9 - Indirapuram Habitat Centre
    // 10 - Aditya Mall
    // 11 - Orange County
    // 12 - NH-9 Ramp / Kala Patthar Junction

    auto addUndirected = [&](int a, int b, double w) {
        engine.addRoad(a, b, w);
        engine.addRoad(b, a, w);
    };

    // --- Noida IT belt (around Sector 62)
    addUndirected(1, 2, 2.5); // Metro <-> Fortis
    addUndirected(2, 6, 2.0); // Fortis <-> Stellar IT Park
    addUndirected(6, 3, 2.0); // Stellar <-> Corenthum
    addUndirected(3, 5, 2.5); // Corenthum <-> Logix
    addUndirected(5, 4, 2.0); // Logix <-> Peace Softtech
    addUndirected(4, 1, 3.0); // Peace Softtech <-> Metro

    // JIIT connections
    addUndirected(7, 2, 2.0); // JIIT <-> Fortis
    addUndirected(7, 3, 1.5); // JIIT <-> Corenthum
    addUndirected(7, 5, 1.8); // JIIT <-> Logix

    // --- Connection towards Indirapuram (via NH-9 ramp) ---
    addUndirected(2, 12, 4.0); // Fortis -> NH-9
    addUndirected(6, 12, 3.5); // Stellar -> NH-9

    // --- Indirapuram side ring ---
    addUndirected(12, 8, 2.5);  // NH-9 ramp <-> Shipra Mall
    addUndirected(8, 9, 1.5);   // Shipra <-> IHC
    addUndirected(9, 10, 1.2);  // IHC <-> Aditya Mall
    addUndirected(10, 12, 2.2); // Aditya Mall <-> NH-9 ramp

    // Inner local roads
    addUndirected(9, 11, 1.0);  // IHC <-> Orange County
    addUndirected(11, 8, 1.0);  // Orange County <-> Shipra

    // Some basic traffic data for congestion API (3 time slots)
    engine.updateTraffic(0, 15);
    engine.updateTraffic(1, 23);
    engine.updateTraffic(2, 10);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        return 0;
    }

    initEngine();
    std::string cmd = argv[1];

    if (cmd == "route") {
        int src = 1;
        if (argc >= 3) src = std::stoi(argv[2]);

        std::vector<double> dist = engine.computeRoute(src);
        int n = static_cast<int>(dist.size()) - 1; // ignore index 0

        std::cout << n;
        for (int i = 1; i <= n; ++i) {
            double d = dist[i];
            if (d == std::numeric_limits<double>::infinity()) {
                std::cout << " " << "inf";
            } else {
                std::cout << " " << d;
            }
        }
        std::cout << std::endl;
    }

    // ---------- emergency-route <src> ----------
    else if (cmd == "emergency-route") {
        int src = 1;
        if (argc >= 3) src = std::stoi(argv[2]);

        engine.addEmergencyRequest(1, src, "ambulance", 1.0);
        std::vector<double> routeDist;
        engine.processNextEmergency(routeDist);

        int n = static_cast<int>(routeDist.size()) - 1;
        std::cout << n;
        for (int i = 1; i <= n; ++i) {
            double d = routeDist[i];
            if (d == std::numeric_limits<double>::infinity()) {
                std::cout << " " << "inf";
            } else {
                std::cout << " " << d;
            }
        }
        std::cout << std::endl;
    }

    // ---------- congestion <start> <end> ----------
    else if (cmd == "congestion") {
        if (argc < 4) {
            std::cout << 0 << std::endl;
            return 0;
        }
        int start = std::stoi(argv[2]);
        int end   = std::stoi(argv[3]);
        int value = engine.getTrafficRange(start, end);
        std::cout << value << std::endl;
    }

    else if (cmd == "parking-status") {
    // zone1: JIIT/Fortis
    // zone2: IT Belt
    // zone3: Indirapuram
    std::cout << "zone1:3/10,zone2:5/8,zone3:7/12" << std::endl;
}
    // ---------- stub: signal-status ----------
    else if (cmd == "signal-status") {
        std::cout << "intersection1:N-S green,E-W red" << std::endl;
    }
 
// ---------- route-path <src> <dest> ----------
// Output format: k v1 v2 ... vk   where k = path length
else if (cmd == "route-path") {
    if (argc < 4) {
        std::cout << 0 << std::endl;
        return 0;
    }

    int src = std::stoi(argv[2]);
    int dest = std::stoi(argv[3]);

    std::vector<int> path = engine.computePath(src, dest);

    std::cout << path.size();
    for (int v : path) {
        std::cout << " " << v;
    }
    std::cout << std::endl;
}
    return 0;
}
