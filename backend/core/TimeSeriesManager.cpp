#include "TimeSeriesManager.h"
#include <algorithm>
TimeSeriesManager::TimeSeriesManager(int slots) {
    init(slots);
}
void TimeSeriesManager::init(int slots) {
    if (slots <= 0) slots = 1;
    n = 1;
    while (n < slots) n <<= 1;   
    tree.assign(2 * n, 0);       
}
void TimeSeriesManager::pointUpdate(int idx, int delta) {
    if (idx < 0 || idx >= n) return;  
    int pos = idx + n;
    tree[pos] += delta;
    for (pos >>= 1; pos >= 1; pos >>= 1) {
        tree[pos] = tree[pos * 2] + tree[pos * 2 + 1];
        if (pos == 1) break;
    }
}
int TimeSeriesManager::rangeQuery(int l, int r) const {
    if (l > r) std::swap(l, r);
    l = std::max(l, 0);
    r = std::min(r, n - 1);
    int res = 0;
    int L = l + n;
    int R = r + n;
    while (L <= R) {
        if (L & 1) res += tree[L++];      // L is a right child
        if (!(R & 1)) res += tree[R--];   // R is a left child
        L >>= 1;
        R >>= 1;
    }
    return res;
}
