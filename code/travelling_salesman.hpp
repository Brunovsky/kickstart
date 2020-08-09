#include "bits.hpp"

// *****

/**
 * Held-Karp dynamic programming exact algorithm
 * Complexity: O(V^2 2^V)
 * Memory: O(V 2^V)
 */
struct travelling_salesman {
    int V;
    vector<vector<int>> dist;

    pair<int, vector<int>> compute() {
        static constexpr int inf = INT_MAX / 2;
        int n = V - 1;
        vector<vector<int>> cost(1 << n, vector<int>(n, inf));
        for (int i = 0; i < n; i++) {
            cost[1 << i][i] = dist[n][i];
        }
        for (int s = 2; s <= n; s++) {
            FOR_EACH_MASK(set, s, n) {
                FOR_EACH_BIT(set, k, kbit) {
                    FOR_EACH_BIT(set ^ kbit, m, mbit) {
                        cost[set][k] = min(cost[set][k],
                                           cost[set ^ kbit][m] + dist[m][k]);
                    }
                }
            }
        }
        // find the optimum and recover the path
        int optimum = inf;
        int set = (1 << n) - 1, k = n;
        for (int i = 0; i < n; i++) {
            if (optimum > cost[set][i] + dist[i][n]) {
                optimum = cost[set][i] + dist[i][n];
                k = i;
            }
        }
        vector<int> path;
        path.push_back(k);
        for (int s = n; s >= 2; s--) {
            int kbit = 1 << k;
            FOR_EACH_BIT(set ^ kbit, m, mbit) {
                if (cost[set][k] == cost[set ^ kbit][m] + dist[m][k]) {
                    path.push_back(m);
                    set ^= kbit;
                    k = m;
                    break;
                }
            }
        }
        path.push_back(n);
        reverse(begin(path), end(path));
        rotate(begin(path), find(begin(path), end(path), 0), end(path));
        return {optimum, path};
    }
};
