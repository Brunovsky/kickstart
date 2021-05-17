#include <bits/stdc++.h>

using namespace std;

// *****

inline namespace lib {

struct linked_lists {
    int L, N;
    vector<int> next, prev;

    // L: lists are [0...L), N: integers are [0...N)
    explicit linked_lists(int L = 0, int N = 0) { assign(L, N); }

    int rep(int l) const { return l + N; }
    int head(int l) const { return next[rep(l)]; }
    int tail(int l) const { return prev[rep(l)]; }
    bool empty(int l) const { return next[rep(l)] == rep(l); }

    void init(int l, int n) { meet(rep(l), n, rep(l)); }
    void clear(int l) { next[rep(l)] = prev[rep(l)] = rep(l); }
    void exchange(int l, int t) {
        assert(!empty(l) && !empty(t));
        int a = next[rep(l)], b = next[rep(t)], c = prev[rep(l)], d = prev[rep(t)];
        meet(rep(t), a), meet(rep(l), b), meet(c, rep(t)), meet(d, rep(l));
    }

    void push_front(int l, int n) { assert(l < L && n < N), meet(rep(l), n, head(l)); }
    void push_back(int l, int n) { assert(l < L && n < N), meet(tail(l), n, rep(l)); }
    void insert_before(int i, int n) { assert(i < N && n < N), meet(prev[i], n, i); }
    void insert_after(int i, int n) { assert(i < N && n < N), meet(i, n, next[i]); }
    void erase(int n) { assert(n < N), meet(prev[n], next[n]); }
    void pop_front(int l) { assert(l < L), meet(rep(l), next[head(l)]); }
    void pop_back(int l) { assert(l < L), meet(prev[tail(l)], rep(l)); }

    void splice_front(int l, int b) { assert(l != b), splice_before(head(l), b); }
    void splice_back(int l, int b) { assert(l != b), splice_after(tail(l), b); }
    void splice_before(int i, int b) {
        if (!empty(b))
            meet(prev[i], head(b)), meet(tail(b), i), clear(b);
    }
    void splice_after(int i, int b) {
        if (!empty(b))
            meet(tail(b), next[i]), meet(i, head(b)), clear(b);
    }

    void clear() {
        iota(begin(next) + N, end(next), N), iota(begin(prev) + N, end(prev), N);
    }
    void assign(int L, int N) {
        this->L = L, this->N = N, next.resize(N + L), prev.resize(N + L), clear();
    }

  private:
    inline void meet(int u, int v) { next[u] = v, prev[v] = u; }
    inline void meet(int u, int v, int w) { meet(u, v), meet(v, w); }
};

#define FOR_EACH_IN_LINKED_LIST(i, l, lists) \
    for (int z##i = l, i = lists.head(z##i); i != lists.rep(z##i); i = lists.next[i])

#define FOR_EACH_IN_LINKED_LIST_REVERSE(i, l, lists) \
    for (int z##i = l, i = lists.tail(z##i); i != lists.rep(z##i); i = lists.prev[i])

template <typename Flow = long, typename FlowSum = Flow>
struct push_relabel {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit push_relabel(int V) : V(V), res(V) {}

    void add(int u, int v, Flow capacity) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0});
        res[v].push_back(E++), edge.push_back({{v, u}, capacity, 0});
    }

    vector<int> height, arc;
    vector<FlowSum> excess;
    linked_lists active, labeled;
    int relabel_count, b; // current bucket (height)
    static constexpr FlowSum sinf = numeric_limits<FlowSum>::max() / 2;

    int global_relabel_threshold() const {
        return 1 + int(ceil(log2(1.5 * E + 1) / log2(V + 1) * V));
    }

    auto reverse_bfs(vector<int>& bfs, vector<int>& new_height) {
        int j = 0, S = 1;
        while (j < S) {
            int v = bfs[j++];
            for (int e : res[v]) {
                int u = edge[e].node[1], r = e ^ 1;
                if (edge[r].flow < edge[r].cap && new_height[u] == 2 * V) {
                    new_height[u] = new_height[v] + 1;
                    bfs.push_back(u), S++;
                }
            }
        }
    }

    void init_bfs(int s, int t) {
        vector<int> bfs{t};
        height.assign(V, 2 * V);
        height[s] = V, height[t] = 0;
        reverse_bfs(bfs, height);
        for (int u : bfs) {
            if (u != t && height[u] < V) {
                labeled.push_back(height[u], u);
            }
        }
    }

    void gap(int at) {
        if (at < V && labeled.empty(at)) {
            for (int h = at + 1; h < V && !labeled.empty(h); h++) {
                FOR_EACH_IN_LINKED_LIST (v, h, labeled) {
                    height[v] = V + 1;
                    if (excess[v] > 0)
                        active.erase(v), active.push_back(V + 1, v);
                }
                labeled.clear(h);
            }
        }
    }

    template <bool sink> // 1=push phase (heights<V), 0=recover phase (heights>V)
    void global_relabel(int s, int t) {
        vector<int> new_height(V, 2 * V), bfs{sink ? t : s};
        new_height[s] = V, new_height[t] = 0;
        reverse_bfs(bfs, new_height);
        for (int u : bfs) {
            if (height[u] != new_height[u]) {
                height[u] = new_height[u];
                if (excess[u] > 0)
                    active.erase(u), active.push_back(height[u], u);
                if (sink && 0 < height[u] && height[u] < V)
                    labeled.erase(u), labeled.push_back(height[u], u);
            }
        }
    }

    void push(int e) {
        auto [u, v] = edge[e].node;
        Flow df = min(excess[u], FlowSum(edge[e].cap - edge[e].flow));
        assert(df > 0);
        if (excess[v] == 0) {
            active.push_back(height[v], v);
        }
        edge[e].flow += df;
        edge[e ^ 1].flow -= df;
        excess[u] -= df;
        excess[v] += df;
    }

    template <bool sink> // 1=push phase (heights<V), 0=recover phase (heights>V)
    void relabel(int u) {
        relabel_count++;
        if (sink) {
            labeled.erase(u);
        }
        height[u] = 2 * V;
        for (int i = 0, vsize = res[u].size(); i < vsize; i++) {
            int e = res[u][i], v = edge[e].node[1];
            if (edge[e].flow < edge[e].cap && height[u] > height[v] + 1) {
                height[u] = height[v] + 1;
                arc[u] = i;
            }
        }
        if (sink && height[u] < V) {
            labeled.push_back(height[u], u);
            // gap(b);
        }
        b = height[u];
    }

    template <bool sink> // 1=push phase (heights<V), 0=recover phase (heights>V)
    void discharge(int u) {
        int& i = arc[u];
        int vsize = res[u].size();
        while (excess[u] > 0) {
            if (i == vsize) {
                relabel<sink>(u);
                if (sink && height[u] >= V) {
                    active.push_back(height[u], u);
                    return;
                }
            }
            int e = res[u][i], v = edge[e].node[1];
            if (edge[e].flow < edge[e].cap && height[u] > height[v]) {
                push(e);
            }
            i += excess[u] > 0;
        }
    }

    FlowSum maxflow(int s, int t, bool value_only = true) {
        excess.assign(V, 0);
        arc.assign(V, 0);
        active.assign(2 * V + 1, V);
        labeled.assign(V, V);

        init_bfs(s, t);
        relabel_count = 0;

        excess[s] = sinf;
        for (int e : res[s]) {
            if (edge[e].cap > 0)
                push(e);
        }
        const int threshold = global_relabel_threshold();

        b = V - 1;
        while (true) {
            if (relabel_count >= threshold) {
                relabel_count = 0;
                global_relabel<1>(s, t);
                b = V - 1;
            }
            b = min(b, V - 1);
            while (b > 0 && active.empty(b))
                b--;
            if (b <= 0)
                break;
            int u = active.tail(b);
            active.pop_back(b);
            discharge<1>(u);
        }

        if (value_only)
            return excess[t];

        b = 2 * V - 1;
        while (true) {
            if (relabel_count >= threshold) {
                relabel_count = 0;
                global_relabel<0>(s, t);
                b = 2 * V - 1;
            }
            while (b > V && active.empty(b))
                b--;
            if (b <= V)
                break;
            int u = active.head(b);
            active.pop_front(b);
            discharge<0>(u);
        }

        return excess[t];
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return height[u] >= V; }
};

template <typename Flow = long, typename FlowSum = Flow>
struct dinitz_flow {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit dinitz_flow(int V) : V(V), res(V) {}

    void add(int u, int v, Flow capacity) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0});
        res[v].push_back(E++), edge.push_back({{v, u}, capacity, 0});
    }

    vector<int> level, arc, Q;
    static constexpr Flow finf = numeric_limits<Flow>::max() / 2;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
        level[s] = 0;
        Q[0] = s;
        int j = 0, S = 1;
        while (j < S) {
            int u = Q[j++];
            for (int e : res[u]) {
                int v = edge[e].node[1];
                if (level[v] == -1 && edge[e].flow < edge[e].cap) {
                    level[v] = level[u] + 1;
                    Q[S++] = v;
                    if (v == t)
                        return true;
                }
            }
        }
        return false;
    }

    auto dfs(int u, int t, Flow mincap) {
        if (u == t) {
            return mincap;
        }
        Flow preflow = 0;
        for (int &i = arc[u], vsize = res[u].size(); i < vsize; i++) {
            int e = res[u][i], v = edge[e].node[1];
            if (edge[e].flow < edge[e].cap && level[u] < level[v]) {
                Flow df = dfs(v, t, min(mincap, edge[e].cap - edge[e].flow));
                edge[e].flow += df;
                edge[e ^ 1].flow -= df;
                preflow += df;
                mincap -= df;
                if (mincap == 0)
                    break;
            }
        }
        return preflow;
    }

    FlowSum maxflow(int s, int t) {
        level.assign(V, 0);
        arc.assign(V, 0);
        Q.resize(V);
        FlowSum max_flow = 0;
        while (bfs(s, t)) {
            max_flow += dfs(s, t, finf);
            fill(begin(arc), end(arc), 0);
        }
        return max_flow;
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

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

    void add(int u, int v, Flow capacity) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0});
        res[v].push_back(E++), edge.push_back({{v, u}, capacity, 0});
    }

    vector<int> level, edges, Q;
    vector<Flow> p;
    vector<FlowSum> h, l;
    static constexpr FlowSum sinf = numeric_limits<FlowSum>::max() / 2;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
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
        h[s] = sinf;
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
        } // 1=push phase, 0=recover phase
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
        level.assign(V, 0);
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

} // namespace lib

auto solve() {
    int N, M;
    cin >> N >> M;

    tidal_flow<int, long> pr(N);

    for (int e = 0; e < M; e++) {
        int u, v, cap;
        cin >> u >> v >> cap, u--, v--;
        if (u != v) {
            pr.add(u, v, cap);
        }
    }

    return pr.maxflow(0, N - 1);
}

// *****

int main() {
    ios::sync_with_stdio(false);
    cout << solve() << endl;
    return 0;
}
