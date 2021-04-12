#ifndef GRAPH_FORMATS_HPP
#define GRAPH_FORMATS_HPP

#include "../formatting.hpp"
#include "graph_operations.hpp"

// *****

string adj_matrix_undirected(const edges_t& g, int V) {
    string head = format("V={}, E={} (undirected)", V, g.size());
    string line(V + 2, '.');
    vector<string> s(V, string(V, ' '));
    for (auto [u, v] : g)
        s[u][v] = s[v][u] = '1';
    return format("{}\n{}\n.{}.\n{}\n", head, line, fmt::join(s, ".\n."), line);
}

string adj_matrix_directed(const edges_t& g, int V) {
    string head = format("V={}, E={} (directed)", V, g.size());
    string line(V + 2, '.');
    vector<string> s(V, string(V, ' '));
    for (auto [u, v] : g)
        s[u][v] = '1';
    return format("{}\n{}\n.{}.\n{}\n", head, line, fmt::join(s, ".\n."), line);
}

string adj_matrix_bipartite(const edges_t& g, int U, int V) {
    string head = format("V={}, E={} (bipartite)", V, g.size());
    string line(V + 2, '.');
    vector<string> s(U, string(V, ' '));
    for (auto [u, v] : g)
        s[u][v] = '1';
    return format("{}\n{}\n.{}.\n{}\n", head, line, fmt::join(s, ".\n."), line);
}

string adj_list_undirected(const edges_t& g, int V) {
    int w = int(log10(V)) + 2;
    vector<string> rows(V);
    for (int u = 0; u < V; u++)
        rows[u] = format("{:>{}} ->", u, w);
    for (auto [u, v] : g)
        rows[u] += format("{:>{}}", v, w), rows[v] += format("{:>{}}", u, w);
    return format("{}\n", fmt::join(rows, "\n"));
}

string adj_list_directed(const edges_t& g, int V) {
    int w = int(log10(V)) + 2;
    vector<string> rows(V);
    for (int u = 0; u < V; u++)
        rows[u] = format("{:>{}} ->", u, w);
    for (auto [u, v] : g)
        rows[u] += format("{:>{}}", v, w);
    return format("{}\n", fmt::join(rows, "\n"));
}

string adj_list_bipartite(const edges_t& g, int U, int V) {
    int w = int(log10(V)) + 2;
    vector<string> rows(U);
    for (int u = 0; u < U; u++)
        rows[u] = format("{:>{}} ->", u, w);
    for (auto [u, v] : g)
        rows[u] += format("{:>{}}", v, w);
    return format("{}\n", fmt::join(rows, "\n"));
}

string compact_simple(const edges_t& g, int V, char sep = ',') {
    int i = 0, w = int(log10(V)) + 1, n = 100 / (2 * w + 1);
    string s;
    for (auto [u, v] : g) {
        s += format("{:>{}}{}{:<{}}", u, w, sep, v, w);
        if (++i == n)
            s.erase(s.find_last_not_of(' ')), s += '\n', i = 0;
    }
    return s + (i ? "\n" : "");
}

string compact_dot(const edges_t& g, int V, bool directed = false) {
    static const char* header[] = {"strict graph", "strict digraph"};
    static const char* arrow[] = {" -- ", " -> "};
    int i = 0, w = int(log10(V)) + 1, n = 96 / (2 * w + 5);
    string s = header[directed] + " {\n"s;
    for (auto [u, v] : g) {
        if (directed || u <= v) {
            s += format("{:>{}} {} {:>{}} ;", u, w + 2, arrow[directed], v, w);
            if (++i == n)
                s += '\n', i = 0;
        }
    }
    return s + (i ? "\n" : "") + "}\n";
}

string to_human_undirected(const edges_t& g, int V) {
    return adj_matrix_undirected(g, V) + adj_list_undirected(g, V);
}

string to_human_directed(const edges_t& g, int V) {
    return adj_matrix_directed(g, V) + adj_list_directed(g, V);
}

string to_human_bipartite(const edges_t& g, int U, int V) {
    return adj_matrix_bipartite(g, U, V) + adj_list_bipartite(g, U, V);
}

string to_simple(const edges_t& g, int V, const string& more, bool directed = false) {
    return format("{} {} {}\n{}", V, g.size(), more, compact_simple(g, directed));
}

string to_string(const edges_t& g, int V) { return to_human_undirected(g, V); }

#endif // GRAPH_FORMATS_HPP
