#ifndef GRAPH_OPERATIONS_HPP
#define GRAPH_OPERATIONS_HPP

#include "graph.hpp"
#include "random.hpp"

// *****

// Adjacency matrix + adjacency list primarily for debugging
template <typename Graph>
string to_string(const Graph& g) {
    stringstream ss;
    ss << "V=" << g.V << ", E=" << g.E << '\n';
    ss << string(g.V + 2, '.') << '\n';
    for (int u = 0; u < g.V; u++) {
        string s(g.V, ' ');
        for (int v : g.adj[u]) {
            s[v] = '1';
        }
        ss << '.' << s << ".\n";
    }
    ss << string(g.V + 2, '.') << '\n';
    for (int u = 0; u < g.V; u++) {
        ss << setw(2) << u << " ->";
        for (int v : g.adj[u]) {
            ss << ' ' << setw(2) << v;
        }
        ss << '\n';
    }
    return ss.str();
}

ostream& operator<<(ostream& out, const graph& g) { return out << to_string(g); }
ostream& operator<<(ostream& out, const digraph& g) { return out << to_string(g); }

// Simple format I use in datasets/ folder
template <typename Graph>
string to_simple(const Graph& g, string after, bool directed = false) {
    stringstream ss;
    ss << g.V << " " << g.E << " " << after << "\n";
    const int w = int(log10(g.V)) + 1;
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            if (directed || u < v)
                ss << setw(w) << u << ' ' << setw(w) << v << "\n";
        }
    }
    ss << "\n";
    return ss.str();
}

// Compact format to use fewer lines and smaller commits
template <typename Graph>
string to_simple_compact(const Graph& g, string after, bool directed = false) {
    stringstream ss;
    ss << g.V << " " << g.E << " " << after << "\n";
    const int w = int(log10(g.V)) + 1;
    int cnt = 0;
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            if (directed || u < v) {
                ss << (cnt ? "  " : "") << setw(w) << u << ' ' << setw(w) << v;
                if (++cnt == 15)
                    ss << "\n", cnt = 0;
            }
        }
    }
    ss << "\n";
    return ss.str();
}

// Dot (graphviz) format for directed or undirected graphs
template <typename Graph>
string to_dot(const Graph& g, bool directed = false) {
    static const char* header[] = {"strict graph", "strict digraph"};
    static const char* arrow[] = {" -- ", " -> "};
    stringstream ss;
    ss << header[directed] << " {\n";
    const int w = int(log10(g.V)) + 1;
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            ss << setw(w) << u << arrow[directed] << setw(w) << v << " ;\n";
        }
    }
    ss << "}\n";
    return ss.str();
}

template <typename Graph>
string to_dot_compact(const Graph& g, bool directed = false) {
    static const char* header[] = {"strict graph", "strict digraph"};
    static const char* arrow[] = {" -- ", " -> "};
    stringstream ss;
    ss << header[directed] << " {\n";
    const int w = int(log10(g.V)) + 1;
    int cnt = 0;
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            if (directed || u < v) {
                ss << (cnt ? "  " : "    ") << setw(w) << u << arrow[directed] << setw(w)
                   << v << " ;";
                if (++cnt == 15)
                    ss << "\n", cnt = 0;
            }
        }
    }
    ss << "}\n";
    return ss.str();
}

// Dimacs format for undirected graphs
template <typename Graph>
string to_dimacs(const Graph& g) {
    stringstream ss;
    ss << "p edge " << g.V << " " << g.E << "\n";
    int edge_count = 0;
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            if (u <= v) {
                ss << "e " << setw(2) << u << " " << setw(2) << v << "\n";
                edge_count++;
            }
        }
    }
    (void)edge_count, assert(edge_count == g.E);
    return ss.str();
}

// Read dimacs format
template <typename Graph = graph>
Graph read_dimacs(ifstream& file) {
    string problem, format;
    int V, E;
    file >> problem >> format >> V >> E >> ws;
    assert(!file.bad() && problem == "p" && format == "edge");
    Graph g(V);
    for (int e = 0; e < E; e++) {
        int u, v;
        file >> format >> u >> v;
        assert(format == "e");
        g.add(u, v);
    }
    return g;
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
 * Shuffle adjacency list
 */
template <typename Graph>
void shuffle_adj(Graph& g) {
    for (int u = 0; u < g.V; u++) {
        shuffle(begin(g.adj[u]), end(g.adj[u]), mt);
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

#endif // GRAPH_OPERATIONS_HPP
