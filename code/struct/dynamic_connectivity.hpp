#pragma once

#include "euler_tour_tree.hpp"

struct dynamic_connectivity {
    struct dynconn_node {
        int subt_size = 0;  // how many nodes in subtree
        int adj[2] = {};    // how many adjacent to this node outside MST/inside MST
        int insubt[2] = {}; // how many adjacent to all nodes inside this subtree

        int tree_size() const { return subt_size; }
        int sum(int tree) const { return insubt[tree]; }

        void pushdown(bool, bool, dynconn_node&, dynconn_node&) {}
        void pushup(bool is_node, bool, dynconn_node& lhs, dynconn_node& rhs) {
            subt_size = is_node + lhs.subt_size + rhs.subt_size;
            insubt[0] = adj[0] + lhs.insubt[0] + rhs.insubt[0];
            insubt[1] = adj[1] + lhs.insubt[1] + rhs.insubt[1];
        }
    };
    int N;
    vector<euler_tour_tree<dynconn_node>> ett;
    unordered_map<pair<int, int>, int> edge_level;
    unordered_map<pair<int, int>, unordered_set<int>> adj[2];

    void ensure_level(int level) {
        if (int(ett.size()) == level) {
            ett.emplace_back(ett[0].t.size() - 1);
        }
    }

    explicit dynamic_connectivity(int N = 0)
        : N(N), ett(1, euler_tour_tree<dynconn_node>(N)) {
        edge_level.reserve(4 * N);
    }

    int num_nodes() const { return N; }

  public:
    // returns true if linking joined two unconnected components
    bool link(int u, int v) {
        if (u == v)
            return false;
        if (ett[0].link(u, v)) {
            add_edge_level<1>(u, v, 0);
            return true;
        } else {
            add_edge_level<0>(u, v, 0);
            return false;
        }
    }

    // returns true if cutting separated a connected component into two
    bool cut(int u, int v) {
        if (u == v)
            return false;
        int level = get_edge_level(u, v);
        if (level == -1)
            return false;
        if (!ett[0].cut(u, v)) {
            rem_edge_level<0>(u, v, level);
            return false;
        }

        for (int i = level; i > 0; i--) {
            bool cut = ett[i].cut(u, v);
            assert(cut);
        }
        rem_edge_level<1>(u, v, level);

        for (int i = level; i >= 0; i--) {
            int u_size = ett[i].access_node(u)->tree_size();
            int v_size = ett[i].access_node(v)->tree_size();
            if (u_size > v_size)
                swap(u, v), swap(u_size, v_size);

            // push tree edges down to level i+1
            while (true) {
                int a = find_with_bridge<1>(u, i);
                if (a == 0)
                    break;
                const auto& edges = adj[1].at({i, a});
                int S = edges.size();
                while (S--) {
                    int b = *edges.begin();
                    rem_edge_level<1>(a, b, i);
                    add_edge_level<1>(a, b, i + 1);
                    ett[i + 1].link(a, b);
                }
            }

            while (true) {
                int a = find_with_bridge<0>(u, i);
                if (a == 0)
                    break;
                const auto& edges = adj[0].at({i, a});
                int S = edges.size();
                while (S--) {
                    int b = *edges.begin();
                    if (ett[i].conn(b, v)) {
                        for (int j = 0; j <= i; j++) {
                            ett[j].link(a, b);
                        }
                        rem_edge_level<0>(a, b, i);
                        add_edge_level<1>(a, b, i);
                        return false;
                    } else {
                        rem_edge_level<0>(a, b, i);
                        add_edge_level<0>(a, b, i + 1);
                    }
                }
            }
        }

        return true;
    }

    void reroot(int u) { ett[0].reroot(u); }

    int findroot(int u) { return ett[0].findroot(u); }

    bool conn(int u, int v) { return ett[0].conn(u, v); }

  private:
    int next_two(int n) const { return n > 1 ? 8 * sizeof(n) - __builtin_clz(n - 1) : 0; }

    int get_level(int u, int v) const {
        auto it = edge_level.find(minmax(u, v));
        return it == edge_level.end() ? -1 : it->second;
    }

    template <bool tree>
    int find_with_bridge(int u, int level) {
        if (ett[level].access_node(u)->sum(tree) == 0) {
            return 0;
        }
        const auto& t = ett[level].t;
        while (t[u].node.adj[tree] == 0) {
            auto [l, r] = t[u].child;
            u = t[l].node.sum(tree) > 0 ? l : r;
        }
        ett[level].access_node(u);
        return u;
    }

    template <bool tree>
    void add_edge_level(int u, int v, int level) {
        ensure_level(level);
        edge_level.emplace(minmax(u, v), level);
        adj[tree][{level, u}].insert(v);
        adj[tree][{level, v}].insert(u);
        ett[level].access_node(u)->adj[tree]++;
        ett[level].access_node(v)->adj[tree]++;
    }

    template <bool tree>
    void rem_edge_level(int u, int v, int level) {
        edge_level.erase(minmax(u, v));
        auto su = adj[tree].find({level, u}), sv = adj[tree].find({level, v});
        su->second.size() == 1u ? adj[tree].erase(su), 0 : su->second.erase(v);
        sv->second.size() == 1u ? adj[tree].erase(sv), 0 : sv->second.erase(u);
        ett[level].access_node(u)->adj[tree]--;
        ett[level].access_node(v)->adj[tree]--;
    }

    int get_edge_level(int u, int v) {
        auto it = edge_level.find(minmax(u, v));
        return it == edge_level.end() ? -1 : it->second;
    }
};
