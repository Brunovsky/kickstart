#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

template <typename Flow = long, typename FlowSum = Flow>
struct tidal_flow {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit tidal_flow(int V) : V(V), res(V) {}

    void add(int u, int v, Flow capacity, bool bothways = false) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0});
        res[v].push_back(E++), edge.push_back({{v, u}, bothways ? capacity : 0, 0});
    }

    vector<int> level, edges, Q;
    vector<Flow> p;
    vector<FlowSum> h, l;
    static constexpr FlowSum flowsuminf = numeric_limits<FlowSum>::max() / 2;

    bool bfs(int s, int t) {
        level.assign(V, -1);
        edges.clear();
        level[s] = 0;
        Q[0] = s;
        int i = 0, S = 1;
        while (i < S && level[Q[i]] != level[t]) {
            int u = Q[i++];
            for (int e : res[u]) {
                int v = edge[e].node[1];
                if (edge[e].flow < edge[e].cap) {
                    if (level[v] == -1) {
                        level[v] = level[u] + 1;
                        Q[S++] = v;
                    }
                    if (level[v] == level[u] + 1) {
                        edges.push_back(e);
                    }
                }
            }
        }
        return level[t] != -1;
    }

    FlowSum tide(int s, int t) {
        fill(begin(h), end(h), 0);
        h[s] = flowsuminf;
        for (int e : edges) {
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(edge[e].cap - edge[e].flow), h[w]);
            h[v] = h[v] + p[e];
        }
        if (h[t] == 0) {
            return 0;
        }
        fill(begin(l), end(l), 0);
        l[t] = h[t];
        for (auto it = edges.rbegin(); it != edges.rend(); it++) {
            int e = *it;
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(p[e]), min(h[w] - l[w], l[v]));
            l[v] -= p[e];
            l[w] += p[e];
        }
        fill(begin(h), end(h), 0);
        h[s] = l[s];
        for (auto e : edges) {
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(p[e]), h[w]);
            h[w] -= p[e];
            h[v] += p[e];
            edge[e].flow += p[e];
            edge[e ^ 1].flow -= p[e];
        }
        return h[t];
    }

    FlowSum maxflow(int s, int t) {
        h.assign(V, 0);
        l.assign(V, 0);
        p.assign(E, 0);
        Q.resize(V);
        FlowSum max_flow = 0, df;
        while (bfs(s, t)) {
            do {
                df = tide(s, t);
                max_flow += df;
            } while (df > 0);
        }
        return max_flow;
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int R, C, N;
    cin >> R >> C >> N;
    vector<array<int, 4>> rooks(N);
    for (auto &[a, b, c, d] : rooks) {
        cin >> a >> c >> b >> d;
        a--, b--, c--, d--;
    }
    // R rows, C column, s, t, 2N internals
    int s = R + C + 2 * N, t = s + 1;
    tidal_flow<int> mf(R + C + 2 * N + 2);
    int ROW = 0, LEFT = R, RIGHT = R + N, COL = R + 2 * N;
    // s to rows
    for (int r = 0; r < R; r++) {
        mf.add(s, r + ROW, 1);
    }
    // rows to internal first
    for (int i = 0; i < N; i++) {
        for (int r = rooks[i][0]; r <= rooks[i][1]; r++) {
            mf.add(r + ROW, i + LEFT, 1);
        }
    }
    // link internal
    for (int i = 0; i < N; i++) {
        mf.add(i + LEFT, i + RIGHT, 1);
    }
    // internal to columns
    for (int i = 0; i < N; i++) {
        for (int c = rooks[i][2]; c <= rooks[i][3]; c++) {
            mf.add(i + RIGHT, c + COL, 1);
        }
    }
    // columns to t
    for (int c = 0; c < C; c++) {
        mf.add(c + COL, t, 1);
    }
    cout << mf.maxflow(s, t) << endl;
    return 0;
}
