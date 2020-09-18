#ifndef GRAPH_FORMATS_HPP
#define GRAPH_FORMATS_HPP

#include "graph.hpp"
#include "graph_operations.hpp"

// *****

string adj_matrix_undirected(const edges_t& g, int V) {
    stringstream ss;
    int E = g.size();
    auto adj = make_adjacency_lists_undirected(g, V);
    ss << "V=" << V << ", E=" << E << '\n';
    ss << string(2 + V, '.') << '\n';
    for (int u = 0; u < V; u++) {
        string s(V, ' ');
        for (int v : adj[u])
            s[v] = '1';
        ss << '.' << s << ".\n";
    }
    ss << string(2 + V, '.') << '\n';
    return ss.str();
}

string adj_matrix_directed(const edges_t& g, int V) {
    stringstream ss;
    int E = g.size();
    auto adj = make_adjacency_lists_directed(g, V);
    ss << "V=" << V << ", E=" << E << '\n';
    ss << string(2 + V, '.') << '\n';
    for (int u = 0; u < V; u++) {
        string s(V, ' ');
        for (int v : adj[u])
            s[v] = '1';
        ss << '.' << s << ".\n";
    }
    ss << string(2 + V, '.') << '\n';
    return ss.str();
}

string adj_matrix_bipartite(const edges_t& g, int U, int V) {
    stringstream ss;
    int E = g.size();
    auto adj = make_adjacency_lists_directed(g, U);
    ss << "U=" << U << ", V=" << V << ", E=" << E << '\n';
    ss << string(2 + V, '.') << '\n';
    for (int u = 0; u < U; u++) {
        string s(V, ' ');
        for (int v : adj[u])
            s[v] = '1';
        ss << '.' << s << ".\n";
    }
    ss << string(2 + V, '.') << '\n';
    return ss.str();
}

string adj_list_undirected(const edges_t& g, int V) {
    stringstream ss;
    auto adj = make_adjacency_lists_undirected(g, V);
    const int w = int(log10(V)) + 2;
    for (int u = 0; u < V; u++) {
        ss << setw(2) << u << " ->";
        for (int v : adj[u]) {
            ss << setw(w) << v;
        }
        ss << '\n';
    }
    return ss.str();
}

string adj_list_directed(const edges_t& g, int V) {
    stringstream ss;
    auto adj = make_adjacency_lists_directed(g, V);
    const int w = int(log10(V)) + 2;
    for (int u = 0; u < V; u++) {
        ss << setw(2) << u << " ->";
        for (int v : adj[u])
            ss << setw(w) << v;
        ss << '\n';
    }
    return ss.str();
}

string adj_list_bipartite(const edges_t& g, int U, int V) {
    stringstream ss;
    auto adj = make_adjacency_lists_directed(g, U);
    const int w = int(log10(V)) + 2;
    for (int u = 0; u < U; u++) {
        ss << setw(2) << u << " ->";
        for (int v : adj[u])
            ss << setw(w) << v;
        ss << '\n';
    }
    return ss.str();
}

string compact_simple(const edges_t& g, int V = 100) {
    stringstream ss;
    const int w = int(log10(V)) + 2;
    int cnt = 0;
    for (auto [u, v] : g) {
        ss << (cnt ? "  " : "") << setw(w) << u << ',' << setw(w) << v;
        if (++cnt == 12)
            ss << "\n", cnt = 0;
    }
    ss << (cnt ? "\n" : "");
    return ss.str();
}

string compact_dot(const edges_t& g, int V = 100, bool directed = false) {
    static const char* header[] = {"strict graph", "strict digraph"};
    static const char* arrow[] = {" -- ", " -> "};
    stringstream ss;
    ss << header[directed] << " {\n";
    const int w = int(log10(V)) + 2;
    int cnt = 0;
    for (auto [u, v] : g) {
        if (directed || u <= v) {
            ss << (cnt ? "  " : "    ") << setw(w) << u << arrow[directed];
            ss << setw(w) << v << " ;";
            if (++cnt == 8)
                ss << "\n", cnt = 0;
        }
    }
    ss << (cnt ? "\n}\n" : "}\n");
    return ss.str();
}

string to_human_undirected(const edges_t& g, int V) {
    stringstream ss;
    ss << adj_matrix_undirected(g, V) << adj_list_undirected(g, V);
    return ss.str();
}

string to_human_directed(const edges_t& g, int V) {
    stringstream ss;
    ss << adj_matrix_directed(g, V) << adj_list_directed(g, V);
    return ss.str();
}

string to_human_bipartite(const edges_t& g, int U, int V) {
    stringstream ss;
    ss << adj_matrix_bipartite(g, U, V) << adj_list_bipartite(g, U, V);
    return ss.str();
}

string to_simple(const edges_t& g, int V, const string& more, bool directed = false) {
    int E = g.size();
    stringstream ss;
    ss << V << ' ' << E << ' ' << more << '\n';
    ss << compact_simple(g, directed);
    return ss.str();
}

string to_string(const edges_t& g, int V) { return to_human_undirected(g, V); }

#endif // GRAPH_FORMATS_HPP
