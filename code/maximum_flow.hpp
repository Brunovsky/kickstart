#include <bits/stdc++.h>

using namespace std;

// *****

struct maximum_flow {
    int V, E, s, t;
    vector<vector<int>> adj;
    vector<int> source;
    vector<int> target;
    vector<int> cap;
    static constexpr int nil = -1, inf = INT_MAX;

    maximum_flow(int V) : V(V), E(0), s(0), t(V + 1) {
        adj.resize(V + 2, {});
    }

    void add(int u, int v, int capacity) {
        assert(0 <= u && u <= V && 1 <= v && v <= V + 1);
        int uv = E++;
        int vu = E++;
        source.resize(E), target.resize(E), cap.resize(E);
        source[uv] = target[vu] = u;
        source[vu] = target[uv] = v;
        cap[uv] = capacity;
        cap[vu] = 0;
        adj[u].push_back(uv);
        adj[v].push_back(vu);
    }

    void add_source(int u) {
        add(s, u, inf);
    }

    void add_sink(int v) {
        add(v, t, inf);
    }

    int compute() {
        assert(s == 0 && t == V + 1);
        vector<int> flow(E, 0);
        int max_flow = 0;
        do {
            vector<int> pred(V + 2, nil);
            queue<int> Q;
            Q.push(s);
            while (!Q.empty()) {
                int u = Q.front();
                Q.pop();
                for (int e : adj[u]) {
                    int v = target[e];
                    if (pred[v] == nil && v != s && cap[e] > flow[e]) {
                        pred[v] = e;
                        Q.push(v);
                    }
                }
            }
            if (pred[t] == nil) {
                break;
            }
            int df = inf;
            for (int e = pred[t]; e != nil; e = pred[source[e]]) {
                df = min(df, cap[e] - flow[e]);
            }
            for (int e = pred[t]; e != nil; e = pred[source[e]]) {
                flow[e] = flow[e] + df;
                flow[e ^ 1] = flow[e ^ 1] - df;
            }
            max_flow += df;
        } while (true);
        return max_flow;
    }
};
constexpr int maximum_flow::nil, maximum_flow::inf;
