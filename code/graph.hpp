#include "hash.hpp"

// *****

template <typename N>
bool vec_erase(vector<N>& vec, const N& n) {
    auto it = find(begin(vec), end(vec), n);
    if (it != end(vec)) {
        iter_swap(it, rbegin(vec));
        vec.pop_back();
        return true;
    }
    return false;
}

static mt19937 mt(random_device{}());
using intd = uniform_int_distribution<int>;
using boold = bernoulli_distribution;

/**
 * Compute the topological hash of a graph
 * Used to arbitrate whether two graphs are isomorphic with negligible probability
 * of false positives and zero probability of false negatives.
 */
struct graph {
    int V, E;
    vector<vector<int>> adj;

    graph(int V = 0) : V(V), E(0) { adj.assign(V, {}); }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V);
        adj[u].push_back(v);
        E++;
    }

    void rem(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V);
        E -= vec_erase(adj[u], v);
    }
};

string to_string(const graph& g) {
    stringstream ss;
    ss << "V=" << g.V << ", E=" << g.E << '\n';
    for (int u = 0; u < g.V; u++) {
        string s(g.V, '0');
        for (int v : g.adj[u]) {
            s[v] = '1';
        }
        ss << s << '\n';
    }
    for (int u = 0; u < g.V; u++) {
        ss << setw(2) << u << " ->";
        for (int v : g.adj[u]) {
            ss << ' ' << setw(2) << v;
        }
        ss << '\n';
    }
    return ss.str();
}

string to_dot(const graph& g, bool directed = false) {
    static const char* header[] = {"strict graph", "strict digraph"};
    static const char* arrow[] = {" -- ", " -> "};
    stringstream ss;
    ss << header[directed] << " {\n";
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            ss << setw(6) << u << arrow[directed] << setw(2) << v << " ;\n";
        }
    }
    ss << "}\n";
    return ss.str();
}

ostream& operator<<(ostream& out, const graph& g) { return out << to_string(g); }

/**
 * Reverse the edges of the graph, and return a new graph.
 * Complexity: O(V + E)
 */
graph reverse(const graph& g) {
    int V = g.V;
    graph rev(V);
    for (int u = 0; u < V; u++) {
        for (int v : g.adj[u]) {
            rev.add(v, u);
        }
    }
    return rev;
}

/**
 * Check whether the entire graph is reachable from one of its vertices s.
 * Complexity: O(V + E)
 */
bool is_connected_bfs(const graph& g, int s = 0) {
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
bool is_connected(const graph& g, bool directed) {
    if (!directed)
        return is_connected_bfs(g);
    else
        return is_connected_bfs(g) && is_connected_bfs(reverse(g));
}

/**
 * Relabel the nodes of this graph randomly, and return a new graph.
 * Complexity: O(V + E)
 */
graph relabel(const graph& g) {
    int V = g.V;
    vector<int> label(V);
    iota(begin(label), end(label), 0);
    shuffle(begin(label), end(label), mt);
    graph h(V);
    for (int u = 0; u < V; u++) {
        for (int v : g.adj[u]) {
            h.add(label[u], label[v]);
        }
    }
    return h;
}

/**
 * Join two graphs together.
 * The new graph has the two graphs joined as disconnected subgraphs.
 * Complexity: O(V + E)
 */
graph& join(graph& g, const graph& h) {
    int n = g.V, V = g.V + h.V;
    g.V = V;
    g.adj.resize(V);
    for (int u = 0; u < h.V; u++) {
        for (int v : h.adj[u]) {
            g.add(u + n, v + n);
        }
    }
    return g;
}

/**
 * Simple algorithm to generate a random k-regular undirected graph (degree of every
 * node is k) meeting the configuration criteria. Fails with non-negligible
 * probability, and automatically tries again.
 */
graph generate_regular(int V, int k, bool connected = true, bool self_loop = false) {
    assert(k + !self_loop <= V && (k > 3 || !connected));
    vector<int> nodes;
    vector<vector<bool>> edges;
    graph g;

restart:
    nodes.resize(V);
    iota(begin(nodes), end(nodes), 0);
    edges.assign(V, vector<bool>(V, false));
    g = graph(V);

    auto cmp = [&](int u, int v) {
        return g.adj[u].size() < g.adj[v].size();
    };

    while (k && !nodes.empty()) {
        sort(begin(nodes), end(nodes), cmp);
        int u = nodes[0];
        int v = -1;
        shuffle(begin(nodes), end(nodes), mt);
        for (int w : nodes) {
            if (!edges[u][w] && (self_loop || u != w)) {
                v = w;
                goto add_edge;
            }
        }
        goto restart;

    add_edge:
        g.add(u, v);
        if (u != v) {
            g.add(v, u);
        }
        edges[u][v] = edges[v][u] = true;

        int degu = g.adj[u].size();
        int degv = g.adj[v].size();
        if (degu == k)
            vec_erase(nodes, u);
        if (degv == k)
            vec_erase(nodes, v);
    }

    if (connected && !is_connected(g, false)) {
        goto restart;
    }
    return g;
}

/**
 * Simple algorithm to generate a random uniform-degree graph where each edge exists
 * with some probability p and each self loop exists with probability s.
 */
graph generate_uniform(int V, double p, double s = 0, bool connected = true,
                       bool directed = false) {
    boold distp(p), dists(s);
    vector<vector<bool>> edges;
    graph g;

restart:
    g = graph(V);
    edges.assign(V, vector<bool>(V, false));

    for (int u = 0; u < V; u++) {
        for (int v = 0; v < V; v++) {
            if (!edges[u][v] && u != v ? distp(mt) : dists(mt)) {
                g.add(u, v);
                edges[u][v] = true;
                if (!directed && u != v && !edges[v][u]) {
                    g.add(v, u);
                    edges[v][u] = true;
                }
            }
        }
    }

    if (connected && !is_connected(g, directed)) {
        goto restart;
    }
    return g;
}

/**
 * Generate a directed acyclic graph, uniformly. Each edge exists with probability p.
 */
graph generate_uniform_dag(int V, double p, bool connected = true) {
    boold distp(p);
    graph g;

restart:
    g = graph(V);

    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            if (distp(mt)) {
                g.add(u, v);
            }
        }
    }

    if (connected && !is_connected_bfs(g, 0)) {
        goto restart;
    }
    return g;
}

/**
 * Generate an arbitrary degree tree.
 * The root of the returned tree is vertex 0.
 * Complexity: O(V)
 */
graph generate_tree(int V, bool toparent = true, bool tochild = false) {
    assert(toparent || tochild);
    graph g(V);
    for (int u = 1; u < V; u++) {
        intd dist(0, u - 1);
        int v = dist(mt);
        if (toparent)
            g.add(v, u);
        if (tochild)
            g.add(u, v);
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
template <typename Fn, typename Gn>
graph generate_scc_expansion(const graph& dag, Fn&& f, Gn&& h) {
    int V = dag.V;
    vector<int> intv(V + 1, 0);
    graph g(0);

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

graph generate_scc_uniform_expansion(const graph& dag, int k, double p) {
    vector<int> cnt(dag.V);
    for (int u = 0; u < dag.V; u++) {
        cnt[u] = 1 + mt() % k;
    }
    auto f = [&](int u) {
        return generate_uniform(cnt[u], p, true, true);
    };
    auto h = [&](int u, int v) {
        return ceil(sqrt(cnt[u] * cnt[v]));
    };
    return generate_scc_expansion(dag, f, h);
}
