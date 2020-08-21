#include "hash.hpp"

// *****

#define debug(...) cerr << "[ " #__VA_ARGS__ ": " << __VA_ARGS__ << " ]\n"

template <typename N>
ostream& operator<<(ostream& out, const vector<N>& vec) {
    int n = vec.size();
    out << "[ ";
    for (int i = 0; i < n; i++) {
        out << vec[i] << " ";
    }
    return out << "]\n";
}

constexpr int inf = INT_MAX / 2;

/**
 * General maximum matching algorithm by Micali and Vazirani
 * Complexity: O(E V^1/2) with a huge constant
 * Based on: "The General Maximum Matching Algorithm of Micali and Vazirani",
 *     Peterson P.A. and Loui M.C.
 * Still testing. Looking for good datasets. Disjoint set will wait in the meantime.
 */
struct micali_vazirani {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source, target;
    vector<int> mate;

    explicit micali_vazirani(int V = 0) : V(V), E(0), adj(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        adj[u].push_back(E);
        adj[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        E++;
    }

    vector<int> base, peak;                // blossoms' base and peak
    vector<int> evenlevel, oddlevel;       // vertex levels
    vector<int> bloom;                     // vertex bloom
    vector<bool> used, vis;                // used and visited edges
    vector<bool> erased, seen;             // erased and visited vertices
    vector<int> parent, mark, marked, cnt; // parent of a vertex, mark
    vector<vector<int>> pred, succ, anom;  // predecessors, successors and anomalies
    vector<vector<int>> candidates, bridges;
    unordered_map<pair<int, int>, int, pair_hasher> edge;

    bool outer(int u) { return (level(u) % 2) == 0; }
    bool inner(int u) { return (level(u) % 2) == 1; }
    bool exposed(int u) { return mate[u] == -1; }
    int level(int u) { return min(oddlevel[u], evenlevel[u]); }
    int minlevel(int u) { return min(oddlevel[u], evenlevel[u]); }
    int maxlevel(int u) { return max(oddlevel[u], evenlevel[u]); }
    int even2(int u, int v) { return evenlevel[u] + evenlevel[v]; }
    int odd2(int u, int v) { return oddlevel[u] + oddlevel[v]; }

    void tag(int u, int side) {
        if (!mark[u])
            marked.push_back(u);
        mark[u] = side;
    }

    int find(int u) {
        while (bloom[u] != -1) {
            assert(u != base[bloom[u]]);
            u = base[bloom[u]];
        }
        return u;
    }

    void init() {
        base.clear(), peak.clear();
        evenlevel.assign(V, inf);
        oddlevel.assign(V, inf);
        bloom.assign(V, -1);
        used.assign(E, false);
        vis.assign(E, false);
        erased.assign(V, false);
        seen.assign(V, false);
        parent.assign(V, -1);
        mark.assign(V, 0);
        marked.clear();
        cnt.assign(V, 0);
        for (int u = 0; u < V; u++) {
            pred[u].clear(), succ[u].clear(), anom[u].clear();
            candidates[u].clear(), bridges[u].clear();
        }
        i = 0;
    }

    void bfs_even() {
        for (int v : candidates[i]) {
            assert(evenlevel[v] == i);
            for (int e : adj[v]) {
                int u = other(e, v);
                if (evenlevel[u] < inf)
                    bridges[even2(u, v) >> 1].push_back(e);
                else {
                    if (oddlevel[u] == inf)
                        oddlevel[u] = i + 1;
                    if (oddlevel[u] == i + 1) {
                        cnt[u]++;
                        pred[u].push_back(e), succ[v].push_back(e);
                        candidates[i + 1].push_back(u);
                    }
                    if (oddlevel[u] < i)
                        anom[u].push_back(e);
                }
            }
        }
    }

    void bfs_odd() {
        for (int v : candidates[i]) {
            assert(oddlevel[v] == i);
            if (bloom[v] != -1)
                continue;
            assert(mate[v] != -1);
            int e = mate[v], u = other(e, v);
            if (oddlevel[u] < inf)
                bridges[odd2(u, v) >> 1].push_back(e);
            if (evenlevel[u] == inf) {
                evenlevel[u] = i + 1;
                cnt[u] = 1;
                pred[u] = {e}, succ[v] = {e};
                candidates[i + 1].push_back(u);
            }
        }
    }

    using path_t = list<int>;
    int dcv, barrier, vL, vR, top, s, t, i; // bfs data

    bool dfs_left() {
        for (int e : pred[vL]) {
            int u = other(e, vL);
            if (used[e] || erased[u])
                continue;
            used[e] = true;
            if (bloom[u] != -1)
                u = find(bloom[u]);
            if (!mark[u]) {
                tag(u, -1);
                parent[u] = e;
                vL = u;
                return false;
            }
        }
        if (vL == s)
            return true;
        vL = parent[vL];
        return false;
    }

    bool dfs_right() {
        for (int e : pred[vR]) {
            int u = other(e, vR);
            if (used[e] || erased[u])
                continue;
            used[e] = true;
            if (bloom[u] != -1)
                u = find(bloom[u]);
            if (!mark[u]) {
                tag(u, +1);
                parent[u] = vR;
                vR = u;
                return false;
            }
        }
        if (vR == barrier) {
            vR = barrier = dcv;
            tag(vR, +1);
            vL = parent[vL];
        } else
            vR = parent[vR];
        return false;
    }

    int bloss_augment() {
        if (bloom[s] != -1 && bloom[s] == bloom[t])
            return 0;
        vL = bloom[s] != -1 ? find(bloom[s]) : s;
        vR = bloom[t] != -1 ? find(bloom[t]) : t;
        marked.clear();
        tag(vL, -1), tag(vR, +1);
        dcv = -1, barrier = vR;

        bool found = false;
        while ((!exposed(vL) || !exposed(vR)) && !found) {
            found = level(vL) >= level(vR) ? dfs_left() : dfs_right();
        }
        if (!found && exposed(vL) && exposed(vR)) {
            auto path = findpath(s, vL, -1, false /* vL to s */);
            path.splice(end(path), findpath(t, vR, -1, true /* t to vR */));
            augment(path), erase(path);
            return 1;
        }
        if (dcv != -1)
            mark[dcv] = 0;
        int B = base.size();
        peak.push_back(top);
        base.push_back(dcv);
        for (int y : marked) {
            if (y == dcv || !mark[y])
                continue;
            bloom[y] = B;
            if (outer(y)) {
                oddlevel[y] = 2 * i + 1 - evenlevel[y];
            } else {
                evenlevel[y] = 2 * i + 1 - oddlevel[y];
                candidates[evenlevel[y]].push_back(y);
                for (int e : anom[y]) {
                    int z = other(e, y);
                    bridges[even2(y, z) >> 1].push_back(e);
                    used[e] = true;
                }
            }
        }
        return 0;
    }

    int find_unvisited_predecessor_edge(int& v) {
        for (int e : pred[v])
            if (!vis[e])
                return e;
        assert(parent[v] != -1);
        return find_unvisited_predecessor_edge(v = parent[v]);
    }

    path_t findpath(int hi, int lo, int b, bool hitolo) {
        assert(level(hi) >= level(lo));
        if (hi == lo)
            return {hi};

        int v = hi;
        while (v != lo) {
            int e = find_unvisited_predecessor_edge(v);
            int u = other(e, v);
            if (bloom[v] == b || bloom[v] == -1)
                vis[e] = true;
            else {
                u = find(bloom[v]);
                if (!vis[u] && !erased[u] && level(u) > level(lo) &&
                    mark[u] == mark[hi]) {
                    seen[u] = true;
                    parent[u] = v, v = u;
                }
            }
        }

        path_t path;
        int x = lo;
        do {
            hitolo ? path.push_front(x) : path.push_back(x);
            x = parent[x];
        } while (x != -1);
        for (auto it = begin(path); next(it) != end(path);) {
            x = *it;
            if (bloom[x] != -1 && bloom[x] != b) {
                open_bloom(path, it, x, hitolo /* x to b <=> hi to lo */);
            } else {
                ++it;
            }
        }
        return path;
    }

    void open_bloom(path_t& path, path_t::iterator& it, int x, bool xtob) {
        int B = bloom[x], b = base[B];
        int p = peak[B], left = source[p], right = target[p];
        it = path.erase(it, next(next(it)));
        if (outer(x)) {
            path.splice(it, findpath(x, b, B, xtob));
        } else if (mark[x] == -1) {
            path.splice(it, findpath(left, x, B, !xtob));
            path.splice(it, findpath(right, b, B, xtob));
        } else {
            path.splice(it, findpath(right, x, B, !xtob));
            path.splice(it, findpath(left, b, B, xtob));
        }
    }

    void augment(path_t& path) {
        auto uit = begin(path);
        auto vit = next(begin(path));
        while (vit != end(path)) {
            int u = *uit++, v = *vit++;
            int e = edge.at({u, v});
            if (mate[v] != e) {
                assert(mate[u] != v);
                mate[u] = mate[v] = e;
            }
        }
        assert_feasible();
    }

    void assert_feasible() {
        vector<bool> n(V, false), m(E, false);
        debug(mate);
        for (int u = 0; u < V; u++) {
            int e = mate[u];
            if (e == -1)
                continue;
            if (n[u])
                assert(m[e] && mate[other(e, u)] == e && other(e, u) < u);
            else {
                int v = other(e, u);
                debug(e), debug(u), debug(v);
                assert(!n[u] && !n[t] && !m[e]);
                assert(u == source[e] || u == target[e]);
                assert(v == source[e] || v == target[e]);
                assert(u != v && mate[u] == mate[v] && !n[u] && !n[v]);
                n[u] = n[v] = m[e] = true;
            }
        }
    }

    void erase(path_t& path) {
        while (!path.empty()) {
            int u = path.back();
            path.pop_back();
            erased[u] = true;
            for (int e : succ[u]) {
                int v = other(e, u);
                if (!erased[v] && --cnt[v] == 0)
                    path.push_front(v);
            }
        }
    }

    int search() {
        for (int u = 0; u < V; u++)
            if (exposed(u))
                evenlevel[u] = 0, candidates[0].push_back(u);
        int augmented = 0;
        while (!candidates[i].empty() && !augmented) {
            (i % 2 == 0) ? bfs_even() : bfs_odd();
            for (uint j = 0; j < bridges[i].size(); j++) {
                top = bridges[i][j], s = source[top], t = target[top];
                if (!erased[s] && !erased[t])
                    augmented += bloss_augment();
            }
            i++;
        }
        return augmented;
    }

    int max_matching() {
        mate.assign(V, -1);
        pred.assign(V, {});
        succ.assign(V, {});
        anom.assign(V, {});
        candidates.assign(V, {});
        bridges.assign(V, {});
        edge.clear();
        for (int e = 0; e < E; e++) {
            edge[{source[e], target[e]}] = edge[{target[e], source[e]}] = e;
        }
        int cardinality = 0, augmented = 1;
        while (cardinality < V - 1 && augmented) {
            init();
            augmented = search();
            cardinality += 2 * augmented;
            fprintf(stderr, "augmentations: %d\n", augmented);
        }
        return cardinality;
    }
};
