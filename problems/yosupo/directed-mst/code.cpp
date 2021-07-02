#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

using edges_t = vector<array<int, 2>>;

struct disjoint_set_rollback {
    int N, S;
    vector<int> next;
    vector<pair<int, int>> history;

    explicit disjoint_set_rollback(int N = 0) : N(N), S(N), next(N, -1) {}

    void assign(int N) { *this = disjoint_set_rollback(N); }
    bool same(int i, int j) { return find(i) == find(j); }
    bool unit(int i) { return next[i] == -1; }
    bool root(int i) { return next[i] < 0; }
    int size(int i) { return -next[i]; }
    int time() const { return history.size(); }

    void rollback(int t) {
        int i = time();
        while (i > t) {
            i--, next[history[i].first] = history[i].second;
            i--, next[history[i].first] = history[i].second;
            S++;
        }
        history.resize(t);
    }

    int find(int i) {
        while (next[i] >= 0) {
            i = next[i];
        }
        return i;
    }

    bool join(int i, int j) {
        i = find(i);
        j = find(j);
        if (i != j) {
            if (size(i) < size(j)) {
                swap(i, j);
            }
            history.emplace_back(i, next[i]);
            history.emplace_back(j, next[j]);
            next[i] += next[j];
            next[j] = i, S--;
            return true;
        }
        return false;
    }
};

template <typename T, typename Compare = less<>>
struct lazy_skew_int_heaps {
    struct node_t {
        int child[2] = {};
        T cost = {}, lazy = {};
    }; // elements are shifted by 1 to allow 0 to be used as a scratchpad
    vector<int> root;
    vector<node_t> node;
    Compare comp;

    explicit lazy_skew_int_heaps(int R = 0, int E = 0, const Compare& comp = Compare())
        : root(R), node(E + 1), comp(comp) {}

    bool empty(int h) const { return root[h] == 0; }
    auto top(int h) {
        pushdown(root[h]);
        return make_pair(root[h] - 1, node[root[h]].cost);
    }
    void update(int h, T delta) {
        assert(!empty(h));
        node[root[h]].lazy += delta;
        pushdown(root[h]);
    }
    void push(int h, int u, T cost) {
        assert(u >= 0), u++;
        node[u].cost = cost;
        root[h] = safe_meld(root[h], u);
    }
    void pop(int h) {
        assert(!empty(h));
        pushdown(root[h]);
        auto [l, r] = node[root[h]].child;
        node[root[h]] = node_t();
        root[h] = safe_meld(l, r);
    }
    void merge(int h, int a, int b) { // merge heaps a and b into position h
        assert(h == a || h == b || root[h] == 0);
        int r = safe_meld(root[a], root[b]);
        root[a] = root[b] = 0, root[h] = r;
    }

  private:
    void pushdown(int a) {
        auto [l, r] = node[a].child;
        node[a].cost += node[a].lazy;
        node[l].lazy += node[a].lazy;
        node[r].lazy += node[a].lazy;
        node[a].lazy = node[0].lazy = 0;
    }
    int safe_meld(int u, int v) {
        if (u == 0 || v == 0 || u == v)
            return u ? u : v;
        return meld(u, v);
    }
    int meld(int a, int b) {
        pushdown(a), pushdown(b);
        if (comp(node[b].cost, node[a].cost)) {
            swap(a, b);
        }
        swap(node[a].child[0], node[a].child[1] = safe_meld(b, node[a].child[1]));
        return a;
    }
};

template <typename Cost = long, typename CostSum = Cost>
optional<pair<CostSum, vector<int>>> min_arborescence(int V, int root,
                                                      const edges_t& edges,
                                                      const vector<Cost>& cost) {
    int E = edges.size();
    disjoint_set_rollback dsu(V);
    lazy_skew_int_heaps<CostSum> heaps(V, E);

    for (int e = 0; e < E; e++) {
        int v = edges[e][1];
        heaps.push(v, e, cost[e]);
    }

    CostSum ans = 0;
    vector<int> seen(V, -1), in(V, -1);
    vector<int> node_path(V), edge_path(V); // growth path
    list<tuple<int, int, vector<int>>> cycles;
    seen[root] = root;

    for (int s = 0; s < V; s++) {
        int u = s, index = 0;
        while (seen[u] == -1) {
            if (heaps.empty(u)) {
                return nullopt; // no edge can reach u
            }
            auto [e, ecost] = heaps.top(u);
            ans += ecost;
            heaps.update(u, -ecost);
            heaps.pop(u);

            edge_path[index] = e;
            node_path[index] = u;
            index++;
            seen[u] = s;
            u = dsu.find(edges[e][0]);

            if (seen[u] == s) {
                int w, end = index;
                int time = dsu.time();
                do {
                    w = node_path[--index];
                    heaps.merge(dsu.find(u), u, w);
                    u = dsu.find(u);
                } while (dsu.join(u, w));
                seen[u] = -1;

                vector<int> cycle(begin(edge_path) + index, begin(edge_path) + end);
                cycles.emplace_front(u, time, move(cycle));
            }
        }
        for (int i = 0; i < index; i++) {
            int e = edge_path[i], v = edges[e][1];
            in[dsu.find(v)] = e;
        }
    }

    for (const auto& [u, time, component] : cycles) {
        dsu.rollback(time);
        int prev = in[u];
        for (int e : component) {
            int v = edges[e][1];
            in[dsu.find(v)] = e;
        }
        int v = edges[prev][1];
        in[dsu.find(v)] = prev;
    }

    vector<int> parent(V);
    parent[root] = root;
    for (int u = 0; u < V; u++) {
        if (in[u] != -1 && u != root) {
            parent[u] = edges[in[u]][0];
        }
    }

    return make_pair(ans, parent);
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int V, E, S;
    cin >> V >> E >> S;
    edges_t g(E);
    vector<int> cost(E);
    for (int e = 0; e < E; e++) {
        cin >> g[e][0] >> g[e][1] >> cost[e];
    }
    auto [mincost, parent] = *min_arborescence<int, long>(V, S, g, cost);
    cout << mincost << '\n';
    for (int u = 0; u < V; u++) {
        cout << parent[u] << " \n"[u + 1 == V];
    }
    return 0;
}
