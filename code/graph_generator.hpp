#include "graph.hpp"

// *****

mt19937 mt(random_device{}());
using intd = uniform_int_distribution<int>;
using longd = uniform_int_distribution<long>;
using reald = uniform_real_distribution<double>;
using binomd = binomial_distribution<int>;
using boold = bernoulli_distribution;

vector<int> get_sparse_sample(int k, int a, int b) {
    assert(a <= b);
    intd dist(a, b);
    vector<int> sample;
    vector<bool> seen(b - a + 1, false);
    while (k--) {
        int n;
        do {
            n = dist(mt);
        } while (seen[n - a]);
        sample.push_back(n);
        seen[n - a] = true;
    }
    sort(begin(sample), end(sample));
    return sample;
}

vector<int> get_dense_sample(int k, int a, int b) {
    vector<int> sample(b - a + 1);
    iota(begin(sample), end(sample), a);
    shuffle(begin(sample), end(sample), mt);
    sample.resize(k);
    sort(begin(sample), end(sample)); // no std::sample in C++14
    return sample;
}

/**
 * Generate k integers in the range [a..b]
 */
vector<int> get_sample(int k, int a, int b) {
    assert(0 <= k && k <= b - a + 1);
    k = min(k, b - a + 1);
    if (k == 0) {
        return {};
    } else if (2 * k <= b - a + 1) {
        return get_sparse_sample(k, a, b);
    } else {
        return get_dense_sample(k, a, b);
    }
}

/**
 * Reverse the edges of the graph, and return a new graph.
 */
digraph reverse(const digraph& g) {
    digraph rev(g);
    swap(rev.adj, rev.rev);
    return rev;
}

/**
 * Relabel the nodes of this graph randomly, and return a new graph.
 */
graph relabel(const graph& g) {
    vector<int> label(g.V);
    iota(begin(label), end(label), 0);
    shuffle(begin(label), end(label), mt);
    graph h(g.V);
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            h.adj[label[u]].push_back(label[v]);
        }
    }
    h.E = g.E;
    return h;
}

digraph relabel(const digraph& g) {
    vector<int> label(g.V);
    iota(begin(label), end(label), 0);
    shuffle(begin(label), end(label), mt);
    digraph h(g.V);
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            h.add(label[u], label[v]);
        }
    }
    h.E = g.E;
    return h;
}

/**
 * Check if a graph is (strongly) connected
 */
template <typename Graph>
int count_reachable(const Graph& g, int s = 0) {
    uint i = 0, S = 1, V = g.V;
    vector<bool> vis(V, false);
    vector<int> bfs{s};
    vis[s] = true;
    while (i++ < S && S < V) {
        for (int v : g.adj[bfs[i - 1]]) {
            if (!vis[v]) {
                vis[v] = true;
                S++;
                bfs.push_back(v);
            }
        }
    }
    return S;
}

template <typename Graph>
bool reachable(const Graph& g, int s, int t) {
    uint i = 0, S = 1, V = g.V;
    vector<bool> vis(V, false);
    vector<int> bfs{s};
    while (i++ < S && S < V) {
        for (int v : g.adj[bfs[i - 1]]) {
            if (!vis[v]) {
                vis[v] = true;
                S++;
                bfs.push_back(v);
                if (v == t)
                    return true;
            }
        }
    }
    return false;
}

bool is_connected(const graph& g) {
    if (g.V == 0)
        return true;
    return count_reachable(g) == g.V;
}

bool is_connected(const digraph& g) {
    if (g.V == 0)
        return true;
    return count_reachable(g) == g.V && count_reachable(reverse(g)) == g.V;
}

bool is_rooted(const digraph& g, int s = 0) {
    if (g.V == 0)
        return true;
    return count_reachable(g, s) == g.V;
}

/**
 * Join two graphs together.
 * The new graph has the two graphs joined as disconnected subgraphs.
 */
graph& join(graph& g, const graph& h) {
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

digraph& join(digraph& g, const digraph& h) {
    int n = g.V, V = g.V + h.V, E = g.E + h.E;
    g.V = V, g.E = E;
    g.adj.resize(V);
    g.rev.resize(V);
    for (int u = 0; u < h.V; u++) {
        for (int v : h.adj[u]) {
            g.adj[u + n].push_back(v + n);
            g.rev[v + n].push_back(u + n);
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
 * Generate a random uniform-degree undirected graph where each edge
 * exists with some probability p. The expected degree of a vertex is pV.
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

/**
 * Generate a random uniform-degree directed graph where each edge
 * exists with some probability p. The expected in and out degree of a vertex is pV.
 */
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

/**
 * Generate a random uniform-degree directed graph where each edge
 * exists with some probability p. Edges are oriented only to vertices with higher
 * label. The expected in-degree of u is up and out-degree is (V-u)p.
 */
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
 * Generate a partition sum of a number V of a given size
 */
vector<int> uniform_rank_partition(int V, int ranks) {
    vector<int> ranksize(ranks);
    for (int i = 0; i < ranks; i++) {
        ranksize[i] = (V + i) / ranks;
    }
    shuffle(begin(ranksize), end(ranksize), mt);
    return ranksize;
}

/**
 * Apply chaos to a partition sum to destroy uniformity
 */
void rank_partition_chaos(vector<int>& ranksize, reald chaos = reald(-0.6, 0.6)) {
    int ranks = ranksize.size();
    for (int i = 1; i < ranks; i++) {
        double q = chaos(mt);
        int a = ranksize[i - 1], b = ranksize[i];
        int c = int(floor(q * (a + b)));
        c = max(-min(a, b), c);
        c = min(min(a, b), c);
        ranksize[i - 1] = a + c;
        ranksize[i] = b - c;
    }
    shuffle(begin(ranksize), end(ranksize), mt);
}

void add_ranked_back_edges(digraph& g, double q, const vector<int>& ranksize) {
    boold distq(q);
    int start = 0, ranks = ranksize.size();
    for (int r = 0; r < ranks; r++) {
        int mid = start + ranksize[r];
        int universe = g.V - mid;
        binomd distk(universe, q);
        for (int u = start; u < mid; u++) {
            int k = distk(mt);
            for (int v : get_sample(k, mid, g.V - 1)) {
                g.add(v, u);
            }
        }
        start = mid;
    }
}

void fill_rank_edges(digraph& g, double p, const vector<int>& ranksize) {
    boold distp(p);
    int start = 0, ranks = ranksize.size();
    for (int r = 0; r < ranks; r++) {
        int mid = start + ranksize[r];
        int universe = g.V - mid;
        binomd distk(universe, p);
        for (int u = start; u < mid; u++) {
            int k = distk(mt);
            for (int v : get_sample(k, mid, g.V - 1)) {
                g.add(u, v);
            }
        }
        start = mid;
    }
}

void fill_null_indegrees(digraph& g, const vector<int>& ranksize) {
    int start = 0, ranks = ranksize.size();
    for (int r = 1; r < ranks; r++) {
        start += ranksize[r - 1];
        int mid = start + ranksize[r];
        intd distu(0, start - 1);
        for (int v = start; v < mid; v++) {
            if (g.rev[v].empty()) {
                g.add(distu(mt), v);
            }
        }
    }
}

void fill_null_outdegrees(digraph& g, const vector<int>& ranksize) {
    int start = 0, ranks = ranksize.size();
    for (int r = 0; r < ranks - 1; r++) {
        int mid = start + ranksize[r];
        intd distv(mid, g.V - 1);
        for (int u = start; u < mid; u++) {
            if (g.adj[u].empty()) {
                g.add(u, distv(mt));
            }
        }
        start = mid;
    }
}

digraph generate_ranked_dag(int V, double p, const vector<int>& ranksize) {
    digraph g(V);
    assert(accumulate(begin(ranksize), end(ranksize), 0) == V);
    fill_rank_edges(g, p, ranksize);
    fill_null_indegrees(g, ranksize);
    fill_null_outdegrees(g, ranksize);
    return g;
}

/**
 * Generate a connected dag where nodes are organized in ranks and edges only go to
 * nodes of greater rank. Each rank has size at least V / ranks.
 * The graph is topologically sorted. Nodes have indegree 0 iff they are in the first
 * rank, and have outdegree 0 iff they are in the last rank.
 * The dag is connected and vertices are topologically sorted (by rank).
 * Each edge exists with probability slightly higher than p to guarantee connectivity.
 */
digraph generate_uniform_ranked_dag(int V, double p, int ranks) {
    auto ranksize = uniform_rank_partition(V, ranks);
    return generate_ranked_dag(V, p, ranksize);
}

/**
 * Generate a connected dag where nodes are organized in ranks and edges only go to
 * nodes of greater rank. The size of each rank is slightly chaotic.
 * The graph is topologically sorted. Nodes have indegree 0 iff they are in the first
 * rank, and have outdegree 0 iff they are in the last rank.
 * The dag is connected and vertices are topologically sorted (by rank).
 * Each edge exists with probability slightly higher than p to guarantee connectivity.
 */
digraph generate_chaos_ranked_dag(int V, double p, int ranks) {
    auto ranksize = uniform_rank_partition(V, ranks);
    rank_partition_chaos(ranksize);
    return generate_ranked_dag(V, p, ranksize);
}

/**
 * Generate a connected digraph where nodes are organized in ranks and edges only go to
 * nodes of greater rank. Each rank has size at least V / ranks.
 * The graph is topologically sorted. Nodes have indegree 0 iff they are in the first
 * rank, and have outdegree 0 iff they are in the last rank.
 * The digraph is connected and vertices are topologically sorted (by rank).
 * connectivity, and each back edge exists with probability q.
 * Each edge exists with probability slightly higher than p to guarantee connectivity.
 */
digraph generate_uniform_ranked_directed(int V, double p, double q, int ranks) {
    auto ranksize = uniform_rank_partition(V, ranks);
    auto g = generate_ranked_dag(V, p, ranksize);
    add_ranked_back_edges(g, q, ranksize);
    return g;
}

/**
 * Generate a connected digraph where nodes are organized in ranks and edges only go to
 * nodes of greater rank. The size of each rank is slightly chaotic.
 * The graph is topologically sorted. Nodes have indegree 0 iff they are in the first
 * rank, and have outdegree 0 iff they are in the last rank.
 * The digraph is connected and vertices are topologically sorted (by rank).
 * Each edge exists with probability slightly higher than p to guarantee connectivity.
 */
digraph generate_chaos_ranked_directed(int V, double p, double q, int ranks) {
    auto ranksize = uniform_rank_partition(V, ranks);
    rank_partition_chaos(ranksize);
    auto g = generate_ranked_dag(V, p, ranksize);
    add_ranked_back_edges(g, q, ranksize);
    return g;
}

/**
 * Generate a connected dag for flow algorithms.
 * The dag is rooted on node 0 which can reach every node, and ends on root V - 1
 * which is reached by every node.
 * The dag is connected and vertices are topologically sorted (by rank).
 * Each edge exists with probability slightly higher than p to guarantee connectivity.
 */
digraph generate_perfect_flow_ranked_dag(int V, double p, int ranks) {
    auto ranksize = uniform_rank_partition(V - 2, ranks);
    rank_partition_chaos(ranksize);
    ranksize.insert(ranksize.begin(), 1);
    ranksize.insert(ranksize.end(), 1);
    auto g = generate_ranked_dag(V, p, ranksize);
    return g;
}

/**
 * Generate a connected digraph for flow algorithms.
 * The digraph is rooted on node 0 which can reach every node, and ends on root V - 1
 * which is reached by every node.
 * The digraph is connected and vertices are topologically sorted (by rank).
 * Each edge exists with probability slightly higher than p to guarantee connectivity.
 */
digraph generate_perfect_flow_ranked(int V, double p, double q, int ranks) {
    auto ranksize = uniform_rank_partition(V - 2, ranks);
    rank_partition_chaos(ranksize);
    ranksize.insert(ranksize.begin(), 1);
    ranksize.insert(ranksize.end(), 1);
    auto g = generate_ranked_dag(V, p, ranksize);
    add_ranked_back_edges(g, q, ranksize);
    return g;
}

/**
 * Generate an arbitrary degree tree rooted at 0.
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

/**
 * Generate an arbitrary degree directed tree rooted at 0.
 */
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
 * Generate undirected grid graph of size WxH vertices
 * Vertices numbered top to bottom, left to right, row-major
 */
graph generate_grid_undirected(int W, int H) {
    int V = W * H;
    graph g(V);
    for (int i = 0; i < W; i++) {
        for (int j = 0; j < H; j++) {
            int u = i * H + j;
            int south = u + H;
            int east = u + 1;
            if (i + 1 < W)
                g.add(u, south);
            if (j + 1 < H)
                g.add(u, east);
        }
    }
    return g;
}

/**
 * Generate directed grid graph of size WxH vertices directed towards southeast
 * Vertices numbered top to bottom, left to right, row-major
 */
digraph generate_grid_directed(int W, int H) {
    int V = W * H;
    digraph g(V);
    for (int i = 0; i < W; i++) {
        for (int j = 0; j < H; j++) {
            int u = i * H + j;
            int south = u + H;
            int east = u + 1;
            if (i + 1 < W)
                g.add(u, south);
            if (j + 1 < H)
                g.add(u, east);
        }
    }
    return g;
}

/**
 * Generate a ranked/level graph where nodes on level i are completely connected to
 * nodes on level i+1 only
 */
graph generate_full_level_undirected(int V, const vector<int>& ranksize) {
    graph g(V);
    int start = 0, ranks = ranksize.size();
    for (int r = 0; r < ranks - 1; r++) {
        int mid = start + ranksize[r];
        int end = mid + ranksize[r + 1];
        for (int u = start; u < mid; u++) {
            for (int v = mid; v < end; v++) {
                g.add(u, v);
            }
        }
    }
    return g;
}

/**
 * Generate a ranked/level graph where nodes on level i are completely connected to
 * nodes on level i+1 only
 */
digraph generate_full_level_directed(int V, const vector<int>& ranksize) {
    digraph g(V);
    int start = 0, ranks = ranksize.size();
    for (int r = 0; r < ranks - 1; r++) {
        int mid = start + ranksize[r];
        int end = mid + ranksize[r + 1];
        for (int u = start; u < mid; u++) {
            for (int v = mid; v < end; v++) {
                g.add(u, v);
            }
        }
    }
    return g;
}

/**
 * Expand each node u of a directed acyclic graph into a randomly generated
 * strongly connected component from f(u).
 * Connect the generated nodes of u into the generated nodes of v in adj[u] using h(u,v)
 * edges and picking nodes arbitrarily.
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
        return int(ceil(sqrt(cnt[u] * cnt[v])));
    };
    return generate_scc_expansion(dag, f, h);
}

/**
 * Transform a directed graph into a flow graph with uniform positive capacities
 * s is 0, t is V - 1.
 */
flow_graph make_flow_graph(const digraph& g, long max_cap) {
    longd capd(1, max_cap);
    flow_graph f(g.V);
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            f.add(u, v, capd(mt));
        }
    }
    return f;
}

/**
 * Generate a random dag flow graph on V vertices
 */
flow_graph generate_dag_flow_graph(int V, double p, long max_cap) {
    intd rankd(3, max(3, V / 3));
    digraph g = generate_perfect_flow_ranked_dag(V, min(1.0, p), rankd(mt));
    return make_flow_graph(g, max_cap);
}

/**
 * Generate a random flow graph on V vertices
 */
flow_graph generate_flow_graph(int V, double p, long max_cap) {
    intd rankd(3, max(3, V / 3));
    digraph g = generate_perfect_flow_ranked(V, min(1.0, p), min(1.0, p), rankd(mt));
    return make_flow_graph(g, max_cap);
}
