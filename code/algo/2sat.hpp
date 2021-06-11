#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Solve 2-SAT in linear time and find strongly connected components.
 *
 * Complexity: O(N)
 * Reference: kactl
 *
 * Usage:
 *   twosat_scc sat(N);                     N variables, [0,N)
 *   sat.set(3);                            Set variable 3 to true
 *   sat.set(~4);                           Set variable 4 to false
 *   sat.either(2,~6);                      var(2) ∨ !var(6)
 *   if (sat.solve()) {
 *     ...                      A solution was found in sat.assignment
 *   } else {
 *     ...                      There is no solution
 *   }
 */
struct twosat_scc {
    int N, C = 0; // N is the number of variables
    vector<unordered_set<int>> adj;
    vector<int> assignment, cmap;

    explicit twosat_scc(int N = 0) : N(N), adj(2 * N) {}

    int add_var() { return adj.resize(2 * N + 2), N++; }

    void either(int u, int v) {
        u = u < 0 ? 2 * ~u : 2 * u + 1; // remove these lines if you want to input
        v = v < 0 ? 2 * ~v : 2 * v + 1; // 2u and 2u+1 directly, with 2n+1 true, 2n false
        assert(0 <= u && u < 2 * N && 0 <= v && v < 2 * N);
        adj[u ^ 1].insert(v);
        adj[v ^ 1].insert(u);
    }

    void set(int u) { either(u, u); }

    vector<int> index, lowlink;
    vector<bool> onstack;
    stack<int> S;
    int depth;

    void tarjan(int u) {
        index[u] = lowlink[u] = depth++;
        S.push(u), onstack[u] = true;

        for (int v : adj[u]) {
            if (!index[v]) {
                tarjan(v);
                lowlink[u] = min(lowlink[u], lowlink[v]);
            } else if (onstack[v]) {
                lowlink[u] = min(lowlink[u], index[v]);
            }
        }

        if (index[u] == lowlink[u]) {
            int v, c = C++;
            do {
                v = S.top(), S.pop(), onstack[v] = false;
                cmap[v] = c;
                if (assignment[v >> 1] == -1) assignment[v >> 1] = v & 1;
            } while (u != v);
        }
    }

    bool solve() {
        assignment.assign(N, -1);
        cmap.assign(2 * N, 0);
        index.assign(2 * N, 0);
        lowlink.assign(2 * N, 0);
        onstack.assign(2 * N, false);
        S = stack<int>();
        depth = 1, C = 0;

        for (int u = 0; u < 2 * N; u++) {
            if (!index[u]) tarjan(u);
        }
        for (int u = 0; u < N; u++) {
            assert(assignment[u] >= 0);
            if (cmap[2 * u] == cmap[2 * u + 1]) return false;
        }
        return true;
    }
};
