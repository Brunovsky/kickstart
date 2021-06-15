#pragma once

#include "../formatting.hpp"
#include "graph_operations.hpp"

edges_t scan_edges(const string& s, char sep = ',') {
    edges_t g;
    stringstream ss(s);
    while (ss) {
        int u, v;
        char dummy;
        ss >> u >> dummy >> v;
        if (ss) {
            g.push_back({u, v});
            assert(dummy == sep);
        }
    }
    return g;
}

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

string simple_dot(const edges_t& g, bool directed) {
    static const char* header[] = {"strict graph", "strict digraph"};
    static const char* arrow[] = {" -- ", " -> "};
    string a = arrow[directed], s = header[directed] + " {\n"s;
    for (auto [u, v] : g) {
        s += format(" {0}{2}{1};\n", u, v, a);
    }
    return s + "}\n";
}

template <typename NodeFn, typename EdgeFn>
string full_dot(const edges_t& g, int lo, int hi, bool directed, NodeFn&& node_annotator,
                EdgeFn&& edge_annotator) {
    static const char* header[] = {"strict graph", "strict digraph"};
    static const char* arrow[] = {" -- ", " -> "};
    int V = hi, w = int(log10(V + 1)) + 1;
    string a = arrow[directed], s = header[directed] + " {\n"s;
    for (int u = lo; u <= hi; u++) {
        auto label = node_annotator(u);
        s += !label.empty() ? format(" {0:>{2}} [{1}];\n", u, label, w)
                            : format(" {0:>{1}};\n", u, w);
    }
    for (auto [u, v] : g) {
        auto label = edge_annotator(u, v);
        s += !label.empty() ? format(" {0:>{4}}{2}{1:>{4}} [{3}];\n", u, v, a, label, w)
                            : format(" {0:>{3}}{2}{1:>{3}};\n", u, v, a, w);
    }
    return s + "}\n";
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
