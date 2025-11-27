#ifndef TIMESERIES_MANAGER_H
#define TIMESERIES_MANAGER_H
#include <vector>
// Simple segment tree for point updates & range queries
class TimeSeriesManager {
private:
    int n;                    
    std::vector<int> tree;    // segment tree array
public:
    // Initialize with required number of time slots
    TimeSeriesManager(int slots = 1440);
    void init(int slots);
    // Increase/decrease a single time slot value
    void pointUpdate(int idx, int delta);
    // Query sum in range [l, r]
    int rangeQuery(int l, int r) const;
};
#endif // TIMESERIES_MANAGER_H
