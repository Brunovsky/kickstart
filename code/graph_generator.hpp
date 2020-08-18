#include "graph.hpp"

// *****

static mt19937 mt(random_device{}());
using intd = uniform_int_distribution<int>;
using boold = bernoulli_distribution;

/**
 * Reverse the edges of the graph, and return a new graph.
 * Complexity: O(V + E)
 */
digraph reverse(const digraph& g) {
    int V = g.V;
    digraph rev(V);
    for (int u = 0; u < V; u++) {
        for (int v : g.adj[u]) {
            rev.add(v, u);
        }
    }
    return rev;
}

/**
 * Relabel the nodes of this graph randomly, and return a new graph.
 * Complexity: O(V + E)
 */
template <typename Graph>
Graph relabel(const Graph& g) {
    int V = g.V;
    vector<int> label(V);
    iota(begin(label), end(label), 0);
    shuffle(begin(label), end(label), mt);
    Graph h(V);
    for (int u = 0; u < V; u++) {
        for (int v : g.adj[u]) {
            h.adj[label[u]].push_back(label[v]);
        }
    }
    h.E = g.E;
    return h;
}

/**
 * Check whether the entire graph is reachable from one of its vertices s.
 * Complexity: O(V + E)
 */
template <typename Graph>
bool is_connected_bfs(const Graph& g, int s = 0) {
    if (g.V == 0)
        return true;
    uint i = 0, V = g.V;
    vector<bool> vis(V, false);
    vector<int> bfs{s};
    vis[s] = true;
    while (i++ < bfs.size()) {
        for (int v : g.adj[bfs[i - 1]]) {
            if (!vis[v]) {
                vis[v] = true;
                bfs.push_back(v);
            }
        }
    }
    return bfs.size() == V;
}

/**
 * Determine if a graph is (strongly) connected.
 * To check strong connectivity in directed graphs we must check the reverse of g too.
 * Complexity: O(V + E)
 */
bool is_connected(const graph& g) { // undirected
    return is_connected_bfs(g);
}
bool is_connected(const digraph& g) { // directed strongly connected
    return is_connected_bfs(g) && is_connected_bfs(reverse(g));
}

/**
 * Join two graphs together.
 * The new graph has the two graphs joined as disconnected subgraphs.
 * Complexity: O(V + E)
 */
template <typename Graph>
Graph& join(Graph& g, const Graph& h) {
    int n = g.V, V = g.V + h.V, E = g.E + h.E;
    g.V = V, g.E = E;
    g.adj.resize(V);
    for (int u = 0; u < h.V; u++) {
        for (int v : h.adj[u]) {
            g.adj[u + n].push_back(v + n);
        }
    }
    return g;
}

/**
 * Simple algorithm to generate a random k-regular undirected graph (degree of every
 * node is k) meeting the configuration criteria. Fails with non-negligible
 * probability, and automatically tries again.
 * Complexity: O(kV^2)
 */
graph generate_regular(int V, int k) {
    assert(3 <= k && k < V && (k % 2 == 0 || V % 2 == 0));
    vector<vector<bool>> edges;
    vector<int> nodes;
    graph g;

    auto cmp = [&](int u, int v) {
        return g.adj[u].size() < g.adj[v].size();
    };

restart:
    nodes.resize(V);
    iota(begin(nodes), end(nodes), 0);
    edges.assign(V, vector<bool>(V, false));
    g = graph(V);

    while (k && !nodes.empty()) {
        nth_element(begin(nodes), begin(nodes), end(nodes), cmp);
        shuffle(begin(nodes) + 1, end(nodes), mt);

        int u = nodes[0], v, vi, S = nodes.size();
        for (int i = 1; i < S; i++) {
            int w = nodes[i];
            if (!edges[u][w] && u != w) {
                vi = i, v = w;
                goto add_edge;
            }
        }
        goto restart;

    add_edge:
        g.add(u, v);
        edges[u][v] = edges[v][u] = true;

        int degu = g.adj[u].size();
        int degv = g.adj[v].size();
        if (degv == k) {
            swap(nodes[vi], nodes.back());
            nodes.pop_back();
        }
        if (degu == k) {
            swap(nodes[0], nodes.back());
            nodes.pop_back();
        }
    }
    return g;
}

/**
 * Simple algorithm to generate a random uniform-degree undirected graph where each edge
 * exists with some probability p.
 */
graph generate_uniform_undirected(int V, double p) {
    boold distp(p);
    graph g(V);
    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            if (distp(mt)) {
                g.add(u, v);
            }
        }
    }
    return g;
}

digraph generate_uniform_directed(int V, double p) {
    boold distp(p);
    digraph g(V);
    for (int u = 0; u < V; u++) {
        for (int v = 0; v < V; v++) {
            if (distp(mt)) {
                g.add(u, v);
            }
        }
    }
    return g;
}

digraph generate_uniform_dag(int V, double p) {
    boold distp(p);
    digraph g(V);
    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            if (distp(mt)) {
                g.add(u, v);
            }
        }
    }
    return g;
}

/**
 * Generate an arbitrary degree tree.
 * The root of the returned tree is vertex 0.
 * Complexity: O(V)
 */
graph generate_tree_undirected(int V) {
    graph g(V);
    for (int u = 1; u < V; u++) {
        intd dist(0, u - 1);
        int v = dist(mt);
        g.add(u, v);
    }
    return g;
}

digraph generate_tree_directed(int V, bool toparent = true, bool tochild = false) {
    digraph g(V);
    for (int u = 1; u < V; u++) {
        intd dist(0, u - 1);
        int v = dist(mt);
        if (toparent)
            g.add(u, v);
        if (tochild)
            g.add(v, u);
    }
    return g;
}

/**
 * Expand each node u of a directed acyclic graph into a randomly generated
 * strongly connected component from f(u).
 * Connect the generated nodes of u into the generated nodes of v in adj[u] using h(u,v)
 * edges and picking nodes arbitrarily.
 * Complexity: O(V + E) as long as f/h don't get too big
 */
template <typename Gn, typename En>
digraph generate_scc_expansion(const digraph& dag, Gn&& f, En&& h) {
    int V = dag.V;
    vector<int> intv(V + 1, 0);
    digraph g(0);

    for (int u = 0; u < V; u++) {
        join(g, f(u));
        intv[u + 1] = g.V;
    }
    for (int u = 0; u < V; u++) {
        for (int v : dag.adj[u]) {
            vector<array<int, 2>> choices;
            for (int su = intv[u]; su < intv[u + 1]; su++) {
                for (int sv = intv[v]; sv < intv[v + 1]; sv++) {
                    choices.push_back({su, sv});
                }
            }
            shuffle(begin(choices), end(choices), mt);
            int edges = h(u, v);
            for (int e = 0; e < edges; e++) {
                g.add(choices[e][0], choices[e][1]);
            }
        }
    }
    return g;
}

digraph generate_scc_uniform_expansion(const digraph& dag, int k, double p) {
    vector<int> cnt(dag.V);
    intd dist(1, k);
    for (int u = 0; u < dag.V; u++) {
        cnt[u] = dist(mt);
    }
    auto f = [&](int u) {
        digraph g;
        do {
            g = generate_uniform_directed(cnt[u], p);
        } while (!is_connected(g));
        return g;
    };
    auto h = [&](int u, int v) {
        return ceil(sqrt(cnt[u] * cnt[v]));
    };
    return generate_scc_expansion(dag, f, h);
}
