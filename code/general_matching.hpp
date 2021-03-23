#ifndef GENERAL_MATCHING_HPP
#define GENERAL_MATCHING_HPP

#include "debug_print.hpp"
#include "graph.hpp"
#include "integer_lists.hpp"

// *****

struct micali_vazirani {
    int V, E = 0;
    vector<int> adj, off, mate;
    edges_t edge;

    micali_vazirani(int V, const edges_t& g)
        : V(V), E(g.size()), adj(2 * E), off(V + 1, 0), mate(V, -1), edge(g) {
        for (auto [u, v] : edge)
            off[u + 1]++, off[v + 1]++;
        inclusive_scan(begin(off), end(off), begin(off));
        vector<int> cur = off;
        int e = 0;
        for (auto [u, v] : edge)
            adj[cur[u]++] = adj[cur[v]++] = e++;
    }

    inline int other(int e, int u) const { return u ^ edge[e][0] ^ edge[e][1]; }
    inline int len(int u) const { return off[u + 1] - off[u]; }

    void node_bootstrap() {
        linked_lists buck(V, V);
        vector<int> cnt(V, 0);
        for (int u = 0; u < V; u++) {
            if (mate[u] == -1 && len(u)) {
                cnt[u] = len(u);
                buck.push_back(cnt[u], u);
            }
        }
        int s = 1;
        while (s < V) {
            if (buck.empty(s)) {
                s++;
                continue;
            }
            int u = buck.head(s);
            buck.erase(u);
            assert(mate[u] == -1);
            for (int i = off[u]; i < off[u + 1]; i++) {
                int e = adj[i], v = other(e, u);
                if (mate[v] == -1) {
                    mate[u] = v, mate[v] = u;
                    buck.erase(v);
                    break;
                }
            }
            if (mate[u] == -1)
                continue;
            for (int w : {u, mate[u]}) {
                for (int i = off[w]; i < off[w + 1]; i++) {
                    int e = adj[i], t = other(e, w);
                    if (mate[t] == -1) {
                        cnt[t]--;
                        buck.erase(t);
                        if (cnt[t] > 0) {
                            buck.push_back(cnt[t], t);
                            s = min(s, cnt[t]);
                        }
                    }
                }
            }
        }
    }

    void edge_bootstrap() {
        forward_lists sumbuck(2 * V, E), minbuck(V, E);
        for (int e = 0; e < E; e++) {
            auto [u, v] = edge[e];
            int s = len(u) + len(v);
            sumbuck.push(s, e);
        }
        for (int s = 2 * V - 1; s >= 0; s--) {
            FOR_EACH_IN_FORWARD_LIST (e, s, sumbuck) {
                auto [u, v] = edge[e];
                int m = min(len(u), len(v));
                minbuck.push(m, e);
            }
        }
        for (int m = 0; m < V; m++) {
            FOR_EACH_IN_FORWARD_LIST (e, m, minbuck) {
                auto [u, v] = edge[e];
                if (mate[u] == -1 && mate[v] == -1)
                    mate[u] = v, mate[v] = u;
            }
        }
    }

    void bootstrap() { node_bootstrap(); }

    int max_matching() {
        int more = 1;
        init();
        while (more && count_matched < V / 2) {
            reset_search();
            more = search();
        }
        return count_matched;
    }

  private:
    static inline constexpr int inf = INT_MAX / 2;
    static inline constexpr int mv_max_phases = 10; // augmentation phases per search

    struct link_t {
        int hi = -1, lo = -1;
    };

    struct node_t {
        int minlevel = inf, maxlevel = inf, level[2] = {inf, inf};
        int vis = -1, bloom = -1, petal = -1;
        link_t trail[2] = {};
        int arc[2] = {}, preds = 0, succs = 0;
        bool color = 0, erased = 0;
    };

    struct bloom_t {
        int peak, base, star;
    };

    struct lists_t {
        vector<int> head, next;

        explicit lists_t(int N = 0, int M = 0) : head(N, -1), next(M) {}

        void push(int h, int n) { next[n] = head[h], head[h] = n; }
        void clear() { fill(begin(head), end(head), -1); }
        void resize(int N, int M) { head.resize(N, -1), next.resize(M); }
    };

    vector<int> pred, succ;
    vector<node_t> node;
    vector<bloom_t> bloom;
    lists_t phaselist, bridges;
    vector<bool> seen;
    int count_matched, phase, blooms, pending, ddfsid, barrier, exposed[2];

    inline void add_phase(int u, int lvl) { phaselist.push(lvl, u), pending++; }
    inline void add_bridge(int e, int lvl) { bridges.push(lvl, e); }

    // find base*(u) in constant time with union-find
    inline int findstar(int u) {
        int b = node[u].bloom;
        if (b != -1) {
            if (node[bloom[b].star].bloom != -1)
                bloom[b].star = findstar(bloom[b].star);
            return bloom[b].star;
        }
        return u;
    }

    void init() {
        pred.resize(2 * E);
        succ.resize(2 * E);
        node.resize(V);
        phaselist.resize(V, V);
        bridges.resize(V, E);

        count_matched = 0;
        for (int u = 0; u < V; u++) {
            assert(mate[u] == -1 || mate[mate[u]] == u);
            count_matched += u < mate[u];
        }
    }

    void reset_search() {
        phase = blooms = ddfsid = pending = 0;
        phaselist.clear(), bridges.clear();
        for (int u = 0; u < V; u++) {
            node[u] = node_t();
            if (mate[u] == -1) {
                add_phase(u, 0);
                node[u].minlevel = node[u].level[0] = 0;
            }
        }
        seen.assign(E, false);
        bloom.clear();
    }

    int search() {
        bool done = false;
        int more, augmentations = 0, good = 0;
        while (!done && good < mv_max_phases && count_matched < V / 2) {
            done = MIN();
            more = MAX();
            good += more > 0;
            count_matched += more;
            augmentations += more;
            phase++;
        }
        return augmentations;
    }

    void visit_prop(int e, int u, int v, bool parity) {
        assert(!seen[e] && !node[u].erased && !node[v].erased);
        if (node[v].minlevel == inf) {
            node[v].minlevel = node[v].level[!parity] = phase + 1;
            add_phase(v, phase + 1);
        }
        assert(node[v].minlevel == phase + 1 && node[v].level[!parity] == phase + 1);
        pred[off[v] + node[v].preds++] = u;
        succ[off[u] + node[u].succs++] = v;
        seen[e] = true;
    }

    void visit_bridge(int e, bool parity) {
        auto [u, v] = edge[e];
        assert(!seen[e] && !node[u].erased && !node[v].erased);
        assert(node[u].level[parity] < inf && node[v].level[parity] < inf);
        int tenacity = node[u].level[parity] + node[v].level[parity] + 1;
        int lvl = tenacity >> 1;
        assert(phase <= lvl && lvl < V);
        add_bridge(e, lvl);
        seen[e] = true;
    }

    void bfs_visit(int e, int u, int v) {
        bool parity = phase % 2;
        if (node[v].minlevel > phase) {
            visit_prop(e, u, v, parity);
        } else if (node[v].level[parity] < inf) {
            visit_bridge(e, parity);
        }
    }

    bool MIN() {
        if (pending == 0)
            return true;

        bool parity = phase % 2;
        for (int u = phaselist.head[phase]; u != -1; pending--, u = phaselist.next[u]) {
            if (node[u].erased)
                continue;
            if (parity == 0) {
                for (int i = off[u]; i < off[u + 1]; i++) {
                    int e = adj[i], v = other(e, u);
                    if (mate[u] != v && !seen[e] && !node[v].erased)
                        bfs_visit(e, u, v);
                }
            } else if (!node[mate[u]].erased) {
                for (int i = off[u]; i < off[u + 1]; i++) {
                    int e = adj[i], v = other(e, u);
                    if (mate[u] == v && !seen[e])
                        bfs_visit(e, u, v);
                    if (mate[u] == v)
                        break;
                }
            }
        }
        return false;
    }

    void push_dfs(int& h, bool c, int v, int w) {
        assert(node[h].vis != ddfsid || (node[h].color == c && node[h].arc[c] > 1));
        node[h].vis = ddfsid;
        node[h].color = c;
        node[w].arc[c] = 0;
        node[w].trail[c].hi = h;
        node[w].trail[c].lo = v;
        h = w;
    }

    bool pop_dfs(int& h, bool c) {
        h = node[h].trail[c].hi;
        int i = node[h].arc[c], s = node[h].preds;
        return i < s;
    }

    void advance_dfs(int& h, bool c) {
        int& i = node[h].arc[c];
        int v = pred[off[h] + i++], w = findstar(v);
        lazy_erase_predecessors(h, i);
        push_dfs(h, c, v, w);
    }

    bool reverse_dfs(int& h, bool c, int b) {
        bool ok = false;
        while (!ok && h != b) {
            ok = pop_dfs(h, c);
        }
        return ok;
    }

    bool backtrack_dfs(int& h, bool c, int b) {
        int x = h, lvl = node[x].minlevel;
        while ((h == x || node[h].minlevel > lvl) && reverse_dfs(h, c, b)) {
            do {
                advance_dfs(h, c);
            } while (node[h].vis != ddfsid && node[h].minlevel > lvl);
        }
        return h != b;
    }

    int ddfs(int peak) {
        int r = findstar(edge[peak][0]), b = findstar(edge[peak][1]);
        if (r == b)
            return -1;

        int red_barrier = r;
        ++ddfsid, barrier = b;
        node[r].arc[0] = node[b].arc[1] = 0;
        node[r].trail[0] = node[b].trail[1] = {};

        while (node[r].minlevel != 0 || node[b].minlevel != 0) {
            if (node[r].minlevel >= node[b].minlevel) {
                advance_dfs(r, 0);
            } else {
                advance_dfs(b, 1);
            }
            if (r == b) {
                if (!backtrack_dfs(b, 1, barrier)) {
                    b = barrier = r;
                    if (!backtrack_dfs(r, 0, red_barrier)) {
                        r = barrier = b;
                        return 0;
                    }
                }
            }
        }
        assert(r != b);
        exposed[0] = r, exposed[1] = b;
        return 1;
    }

    int MAX() {
        int augmentations = 0;
        for (int peak = bridges.head[phase]; peak != -1; peak = bridges.next[peak]) {
            auto [red, blue] = edge[peak];
            if (node[red].erased || node[blue].erased)
                continue;

            auto what = ddfs(peak);
            if (what == 1) {
                augment_path(peak);
                augmentations++;
            } else if (what == 0) {
                form_bloom(peak);
                blooms++;
            }
        }
        return augmentations;
    }

    void bloom_build_petals() {
        for (bool c : {0, 1}) {
            int u = barrier, v = node[u].trail[c].hi;
            while (v != -1) {
                node[v].petal = node[u].trail[c].lo;
                u = v, v = node[u].trail[c].hi;
            }
        }
    }

    void bloom_dfs_level(int u) {
        if (u == barrier)
            return;

        int lvl = 2 * phase + 1 - node[u].minlevel;
        assert(!node[u].erased && node[u].bloom == -1 && node[u].vis == ddfsid);
        assert(lvl < V && node[u].maxlevel == inf && node[u].level[lvl % 2] == inf);

        node[u].bloom = blooms;
        node[u].maxlevel = node[u].level[lvl % 2] = lvl;
        add_phase(u, lvl);

        for (int i = 0; i < node[u].preds; i++) {
            int v = pred[off[u] + i], w = findstar(v);
            if (node[w].bloom == -1 && node[u].color == node[w].color)
                bloom_dfs_level(w);
        }

        if (lvl % 2 == 0) {
            for (int i = off[u]; i < off[u + 1]; i++) {
                int e = adj[i], v = other(e, u);
                if (!node[v].erased && !seen[e] && node[v].level[0] < inf)
                    visit_bridge(e, 0);
            }
        }
    }

    void form_bloom(int peak) {
        assert(node[barrier].bloom == -1);
        bloom.push_back({peak, barrier, barrier});
        bloom_build_petals();
        bloom_dfs_level(findstar(edge[peak][0]));
        bloom_dfs_level(findstar(edge[peak][1]));
    }

    using path_t = list<int>;

    inline void add_path(path_t& path, bool back, path_t&& subpath) {
        path.splice(back ? end(path) : begin(path), subpath);
    }
    inline void add_path(path_t& path, bool back, int node) {
        back ? path.push_back(node) : path.push_front(node);
    }

    path_t walk_bloom(int u, bool down) {
        int B = node[u].bloom;

        if (node[u].minlevel % 2 == 0) /* outer */ {
            return walk_down(u, B, down);
        } else /* inner */ {
            int t = edge[bloom[B].peak][!node[u].color];
            auto path = walk_peak(u, B, !down);
            add_path(path, down, walk_base(t, B, down));
            return path;
        }
    }

    path_t walk_star(int u, int star, bool down) {
        path_t path;
        while (u != star) {
            add_path(path, down, walk_bloom(u, down));
            u = bloom[node[u].bloom].base;
        }
        return path;
    }

    path_t walk_peak(int u, int B, bool down) {
        bool c = node[u].color;
        int t = edge[bloom[B].peak][c];

        path_t path{u}, top_path;
        while (node[t].bloom != B) {
            add_path(top_path, down, walk_bloom(t, down));
            t = bloom[node[t].bloom].base;
        }
        while (u != t) {
            int v = node[u].trail[c].lo;
            add_path(path, !down, walk_star(v, u, down));
            u = node[u].trail[c].hi;
            add_path(path, !down, u);
        }
        add_path(path, !down, move(top_path));
        return path;
    }

    path_t walk_base(int u, int B, bool down) {
        int base = bloom[B].base;

        path_t path;
        while (u != base) {
            if (node[u].bloom == B) {
                add_path(path, down, u);
                u = node[u].petal; // take a predecessor of the same color
            } else {
                add_path(path, down, walk_bloom(u, down));
                u = bloom[node[u].bloom].base;
            }
        }
        return path;
    }

    path_t walk_down(int u, int B, bool down) {
        int base = bloom[B].base;

        path_t path;
        while (u != base) {
            if (node[u].bloom == B) {
                add_path(path, down, u);
                u = pred[off[u]]; // any predecessor works to go down
            } else {
                add_path(path, down, walk_bloom(u, down));
                u = bloom[node[u].bloom].base;
            }
        }
        return path;
    }

    path_t find_path(int top, int c, bool down) {
        int u = exposed[c], w = findstar(top);

        path_t path;
        while (u != w) {
            int v = node[u].trail[c].lo;
            add_path(path, !down, u);
            add_path(path, !down, walk_star(v, u, down));
            u = node[u].trail[c].hi;
        }
        add_path(path, !down, w);
        add_path(path, !down, walk_star(top, w, down));
        return path;
    }

    void augment_path(int peak) {
        auto path = find_path(edge[peak][0], 0, false);
        auto rest = find_path(edge[peak][1], 1, true);
        path.splice(end(path), rest);

        assert(path.size() == 2u * phase + 2);

        auto ait = begin(path), bit = next(ait);
        while (bit != end(path)) {
            int u = *ait++, v = *bit++;
            assert(!node[u].erased && !node[v].erased);
            if (mate[v] != u)
                mate[u] = v, mate[v] = u;
        }
        for (int u : path) {
            node[u].erased = true;
        }
        erase_successors(path);
    }

    void erase_successors(path_t& path) {
        while (!path.empty()) {
            int u = path.front();
            path.pop_front();
            for (int i = 0; i < node[u].succs; i++) {
                int v = succ[off[u] + i];
                if (!node[v].erased && lazy_erase_predecessors(v)) {
                    node[v].erased = true;
                    path.push_back(v);
                }
            }
        }
    }

    inline bool lazy_erase_predecessors(int v, int i = 0) {
        int& s = node[v].preds;
        while (i < s && node[pred[off[v] + i]].erased) {
            swap(pred[off[v] + i], pred[off[v] + s - 1]), s--;
        }
        return i == s; // erased all after i?
    }
};

#endif // GENERAL_MATCHING_HPP
