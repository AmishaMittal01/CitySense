# **CitySense – Smart City Transportation & Infrastructure Simulator**

CitySense is a **C++ based smart-city simulation platform** featuring a fully interactive **Graphical User Interface (GUI)**. It models real urban challenges such as **traffic congestion, parking availability, road networks, and emergency response routing** using data structures, algorithms, and simulation engines.

The GUI allows users to **visualize city movement**, **run simulations**, **inspect graphs**, and **interact with modules in real-time**, making it easy for teachers, evaluators, and developers to understand the system.

---

## **Key Features**

### **1. Interactive GUI (Frontend)**

* Visual representation of:

  * Road networks
  * Parking availability
  * Emergency vehicle routing
  * Live simulation logs
* Simple and modern design for easy demonstration.
* Helps visualize what is happening in the backend simulation engine.

### **2. Traffic Simulation Engine**

* Controls traffic flow based on graph networks.
* Simulates congestion, signals, and vehicle routes.
* Powered by `TrafficController` & `TrafficSimulationEngine`.

### **3. Parking Management System**

* Real-time parking assignment and freeing of slots.
* GUI displays parking status visually.
* Controlled by `ParkingController`.

### **4. Emergency Management**

* Optimized routing for emergency vehicles.
* Uses graph algorithms (shortest path).
* Visualized in GUI with highlighted routes.

### **5. Graph-Based City Model**

* Each intersection/road is represented as nodes & edges.
* Uses algorithms like BFS, DFS, and Dijkstra depending on scenario.
* Managed by `GraphManager`.

### **6. Time-Series & City Analytics**

* Tracks and analyzes time-dependent data (traffic patterns, emergency frequency).
* Managed by `TimeSeriesManager`.

### **7. Integrated Testing**

* `backend/tests` includes end-to-end simulation tests.
* Ensures all modules interact correctly:

  * graph → parking → traffic → emergency → rollback → GUI update

---

## 🧠 **Tech Stack**

### **Backend (Simulation Engine)**

* **C++17**
* **CMake**
* Algorithms Used:

  * Graph (Adjacency lists, weighted edges)
  * Shortest path algorithms
  * Queues & stacks for event simulation
  * Time-series data structures

### **Frontend (GUI)**

* **React.js**
* **Leaflet**
---

## 🎯 **What This Project Demonstrates**


## 🧩 **Future Enhancements**
Live backend → GUI communication via WebSockets,
Real-time IoT sensor data simulation,
Multiple vehicle types (bus, ambulance, two-wheelers),
Predictive ML models (ETA, congestion prediction)

---

## 🤝 **Contributing**
This project was build as a mini project under the Data Structures Lab coursework by:
Harshita Saxena
Sheen Sharma 
Amisha Mittal
