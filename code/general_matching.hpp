#include "debug_print.hpp"
#include "hash.hpp"

// *****

void print_edges(const vector<int>& source, const vector<int>& target) {
    int E = source.size();
    print("{:>{}}    ", "source", PAD - 2);
    for (int e = 0; e < E; e++) {
        print("{:>{}} ", source[e], dw);
    }
    print("\n");
    print("{:>{}}    ", "target", PAD - 2);
    for (int e = 0; e < E; e++) {
        print("{:>{}} ", target[e], dw);
    }
    print("\n");
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

    explicit micali_vazirani(int V = 0) : V(V), E(0), adj(V), mate(V, -1) {}

    int other(int e, int u) {
        return u == target[e] ? source[e] : (assert(u == source[e]), target[e]);
    }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        adj[u].push_back(E);
        adj[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        E++;
    }

    void megadump(string name) {
        dstep(i, name);
        dheader("edges", E);
        print_edges(source, target);
        debug(used);
        debug(vis);
        debug(base);
        debug(peak);
        dheader("nodes", V);
        debug(mate);
        debug(evenlevel);
        debug(oddlevel);
        debug(bloom);
        debug(erased);
        debug(seen);
        debug(parent);
        debug(mark);
        debug(marked);
        debug(cnt);
        debugv(pred);
        debugv(succ);
        debugv(anom);
        debugh(candidates);
        debugh(bridges);
        dflash();
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
    using path_t = list<int>;
    int dcv, barrier, vL, vR, top, s, t, i; // bfs data

    bool outer(int u) { return level(u) != inf && (level(u) % 2) == 0; }
    bool inner(int u) { return level(u) != inf && (level(u) % 2) == 1; }
    bool exposed(int u) { return mate[u] == -1; }
    int level(int u) { return min(oddlevel[u], evenlevel[u]); }
    int minlevel(int u) { return min(oddlevel[u], evenlevel[u]); }
    int maxlevel(int u) { return max(oddlevel[u], evenlevel[u]); }
    int even2(int u, int v) { return evenlevel[u] + evenlevel[v]; }
    int odd2(int u, int v) { return oddlevel[u] + oddlevel[v]; }

    void tag(int u, int side) {
        if (!mark[u])
            marked.push_back(u);
        else
            print("{} has already been marked\n", u);
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
        dcv = barrier = vL = vR = top = s = t = i = 0;
    }

    void bfs_even() {
        dprintin("BFS_EVEN {}\n", i);
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
        dprintout("BFS_EVEN {}\n", i);
    }

    void bfs_odd() {
        dprintin("BFS_ODD {}\n", i);
        for (int v : candidates[i]) {
            assert(oddlevel[v] == i && mate[v] != -1);
            if (bloom[v] != -1)
                continue;
            int e = mate[v], u = other(e, v);
            if (oddlevel[u] < inf)
                bridges[odd2(u, v) >> 1].push_back(e);
            if (evenlevel[u] == inf) {
                evenlevel[u] = i + 1;
                cnt[u]++;
                pred[u].push_back(e), succ[v].push_back(e);
                candidates[i + 1].push_back(u);
            }
        }
        dprintout("BFS_ODD {}\n", i);
    }

    path_t findpath(int hi, int lo, int b) {
        assert(level(hi) >= level(lo));
        dprintin("FINDPATH {} to {}, b={}\n", hi, lo, b);

        if (hi == lo) {
            dprintout("FINDPATH TRIVIAL: {}\n", hi);
            return {hi};
        }

        dheader("nodes", V);
        debug(parent), debug(seen), debug(erased), debug(mark);

        int v = hi, u = hi, e;
        uint j = 0, vsize;
        do {
            for (e = -1, vsize = pred[v].size(); j < vsize; j++) {
                e = pred[v][j];
                if (!vis[e])
                    break;
            }
            if (j == vsize) {
                dprint("backtrack v={}->{}\n", v, parent[v]);
                seen[v] = false, v = parent[v], j = 0;
                continue;
            }
            u = other(e, v);
            if (bloom[v] == b || bloom[v] == -1) {
                vis[e] = true, j++;
                dprint("EDGE e={} u={} v={} j={} bloom[u]={}\n", e, u, v, j, bloom[u]);
            } else {
                u = base[bloom[v]], j = vsize;
                dprint("BLOOM u={} v={} B={}\n", u, v, bloom[v]);
            }

            if (!seen[u] && !erased[u] && level(u) >= level(lo) && mark[u] == mark[hi]) {
                seen[u] = true;
                parent[u] = v, v = u;
                j = 0;
            }
        } while (u != lo);

        dheader("nodes", V);
        debug(parent), debug(seen);

        path_t path;
        int x = lo;
        do {
            path.push_front(x), x = parent[x];
        } while (x != -1);
        dprint("PARENT PATH (hi->lo) {}\n", path);
        for (auto it = begin(path); it != end(path);) {
            x = *it;
            dprint("it={} x={} b[x]={}\n", it == end(path) ? -1 : *it, x, bloom[x]);
            if (bloom[x] != -1 && bloom[x] != b) {
                auto subpath = open_blossom(x);
                it = path.erase(path.erase(it));
                path.splice(it, subpath);
            } else {
                ++it;
            }
        }
        dprintout("FOUND PATH {}\n", path);
        return path;
    }

    path_t open_blossom(int x) {
        int B = bloom[x], b = base[B];
        int p = peak[B], left = source[p], right = target[p];
        dprintin("OPEN IN x={} B={} b={} p={} l={} r={}\n", x, B, b, p, left, right);
        path_t path;
        if (outer(x)) {
            dprint("find through outer\n");
            path = findpath(x, b, B);
        } else if (mark[x] == -1) {
            dprint("find through inner left\n");
            path = findpath(left, x, B);
            reverse(begin(path), end(path));
            path.splice(end(path), findpath(right, b, B));
        } else {
            dprint("find through inner right\n");
            path = findpath(right, x, B);
            reverse(begin(path), end(path));
            path.splice(end(path), findpath(left, b, B));
        }
        dprintout("OPEN OUT {} x={} B={} b={}\n", path, x, B, b);
        return path;
    }

    void augment(path_t& path) {
        dprintin("AUGMENT {}\n", path);
        dheader("nodes", V);
        debug(mate);
        auto uit = begin(path);
        auto vit = next(begin(path));
        while (vit != end(path)) {
            int u = *uit++, v = *vit++;
            int e = edge.at({u, v});
            if (mate[v] != e) {
                assert(mate[u] != e);
                mate[u] = mate[v] = e;
            }
        }
        dprintout("CHECKING OK\n");
        debug(mate);
        assert_feasible();
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

    void assert_feasible() {
        vector<bool> n(V, false), m(E, false);
        for (int u = 0; u < V; u++) {
            int e = mate[u];
            if (e == -1) {
                n[u] = true;
                continue;
            }
            if (n[u])
                assert(m[e] && mate[other(e, u)] == e && other(e, u) < u);
            else {
                int v = other(e, u);
                assert(!n[u] && !n[v] && !m[e]);
                assert(u == source[e] || u == target[e]);
                assert(v == source[e] || v == target[e]);
                assert(u != v && mate[u] == mate[v] && !n[u] && !n[v]);
                n[u] = n[v] = m[e] = true;
            }
        }
    }

    bool dfs_left() {
        for (int e : pred[vL]) {
            int u = other(e, vL);
            if (used[e] || erased[u])
                continue;
            used[e] = true;
            u = find(u);
            if (!mark[u]) {
                dprint("DFS_LEFT  tagged {} -1 on edge {}\n", u, e);
                tag(u, -1);
                parent[u] = vL;
                vL = u;
                return false;
            }
        }
        if (vL == s) {
            return true;
        }
        dprint("DFS_LEFT  backtrack on vL!=s: {} -> {}\n", vL, parent[vL]);
        vL = parent[vL];
        return false;
    }

    bool dfs_right() {
        for (int e : pred[vR]) {
            int u = other(e, vR);
            if (used[e] || erased[u])
                continue;
            used[e] = true;
            u = find(u);
            if (!mark[u]) {
                dprint("DFS_RIGHT tagged {} +1 on edge {}\n", u, e);
                tag(u, +1);
                parent[u] = vR;
                vR = u;
                return false;
            } else if (u == vL)
                dcv = u;
        }
        if (vR == barrier) {
            dprint("DFS_RIGHT backtrack on vR==barrier, vL:{}->{}\n", vL, parent[vL]);
            assert(vL == dcv);
            vR = barrier = dcv;
            tag(vR, +1);
            vL = parent[vL];
        } else {
            dprint("DFS_RIGHT backtrack on vR!=barrier: vR:{}->{}\n", vR, parent[vR]);
            vR = parent[vR];
        }
        return false;
    }

    int bloss_augment() {
        if (bloom[s] != -1 && bloom[s] == bloom[t])
            return 0;
        vL = find(s);
        vR = find(t);
        if (vL == vR)
            return 0;
        if (vL != s)
            parent[vL] = s;
        if (vR != t)
            parent[vR] = t;
        marked.clear();
        tag(vL, -1), tag(vR, +1);
        dcv = -1, barrier = vR;

        dprintin("BLOSS_AUGMENT vL={} vR={} top={} s={} t={}\n", vL, vR, top, s, t);

        bool f = false;
        while (!(exposed(vL) && exposed(vR)) && !f) {
            f = level(vL) >= level(vR) ? dfs_left() : dfs_right();
            dprint("LOOP vL={} vR={} dcv={} barrier={} f={}\n", vL, vR, dcv, barrier, f);
        }
        if (exposed(vL) && exposed(vR)) {
            dprint("~ PATH vL={} vR={} dcv={} barrier={}\n", vL, vR, dcv, barrier);
            auto path = findpath(s, vL, -1);
            reverse(begin(path), end(path));
            path.splice(end(path), findpath(t, vR, -1));
            augment(path), erase(path);
            dprintout("AUGMENTATION!\n");
            return 1;
        }
        dprintout("BLOSSOM vL={} vR={} dcv={} barrier={}\n", vL, vR, dcv, barrier);
        assert(dcv != -1);
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

    int search() {
        for (int u = 0; u < V; u++)
            if (exposed(u))
                evenlevel[u] = 0, candidates[0].push_back(u);
        int augmented = 0;
        while (!candidates[i].empty() && !augmented) {
            megadump("before bfs, after previous bloss");
            (i % 2 == 0) ? bfs_even() : bfs_odd();
            megadump("after bfs, before bloss");
            for (uint j = 0; j < bridges[i].size(); j++) {
                top = bridges[i][j], s = source[top], t = target[top];
                if (!erased[s] && !erased[t]) {
                    augmented += bloss_augment();
                }
            }
            i++;
        }
        megadump("done");
        return augmented;
    }

    int max_matching() {
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
