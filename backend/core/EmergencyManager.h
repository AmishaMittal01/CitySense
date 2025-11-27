#ifndef EMERGENCY_MANAGER_H
#define EMERGENCY_MANAGER_H
#include <queue>
#include <string>
#include <vector>
#include <functional>

struct EmergencyRequest {
    int id;                 
    int sourceNode;         
    std::string type;       
    double priority;       
};

struct EmergencyCompare {
    bool operator()(const EmergencyRequest &a,
                    const EmergencyRequest &b) const {
        if (a.priority == b.priority)
            return a.id > b.id; 
        return a.priority > b.priority;    
    }
};

class EmergencyManager {
private:
    std::priority_queue<
        EmergencyRequest,
        std::vector<EmergencyRequest>,
        EmergencyCompare
    > pq;

public:
    EmergencyManager() = default;
    void addEmergency(int id, int sourceNode, const std::string &type, double priority);
    bool hasEmergency() const;
    EmergencyRequest peekEmergency() const;
    EmergencyRequest getNextEmergency();
};
#endif // EMERGENCY_MANAGER_H
