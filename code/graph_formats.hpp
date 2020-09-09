#ifndef GRAPH_FORMATS_HPP
#define GRAPH_FORMATS_HPP

#include "graph.hpp"

// *****

int len(const graph& g) { return g.V; }
int len(const digraph& g) { return g.V; }
int len(const bipartite_graph& g) { return g.U; }

template <typename Graph>
string adj_matrix(const Graph& g) {
    stringstream ss;
    ss << "V=" << g.V << ", E=" << g.E << '\n';
    ss << string(2 + g.V, '.') << '\n';
    for (int u = 0; u < g.V; u++) {
        string s(g.V, ' ');
        for (int v : g.adj[u]) {
            s[v] = '1';
        }
        ss << '.' << s << ".\n";
    }
    ss << string(2 + g.V, '.') << '\n';
    return ss.str();
}

string adj_matrix(const bipartite_graph& g) {
    stringstream ss;
    ss << "U=" << g.U << ", V=" << g.V << ", E=" << g.E << '\n';
    ss << string(2 + g.V, '.') << '\n';
    for (int u = 0; u < g.U; u++) {
        string s(g.V, ' ');
        for (int v : g.adj[u]) {
            s[v] = '1';
        }
        ss << '.' << s << ".\n";
    }
    ss << string(2 + g.V, '.') << '\n';
    return ss.str();
}

template <typename Graph>
string adj_list(const Graph& g) {
    stringstream ss;
    for (int u = 0; u < len(g); u++) {
        ss << setw(2) << u << " ->";
        for (int v : g.adj[u]) {
            ss << ' ' << setw(2) << v;
        }
        ss << '\n';
    }
    return ss.str();
}

template <typename Graph>
string compact_simple(const Graph& g, bool directed = false) {
    stringstream ss;
    const int w = int(log10(len(g))) + 2;
    int cnt = 0;
    for (int u = 0; u < len(g); u++) {
        for (int v : g.adj[u]) {
            if (directed || u <= v) {
                ss << (cnt ? "  " : "") << setw(w) << u << ',' << setw(w) << v;
                if (++cnt == 12)
                    ss << "\n", cnt = 0;
            }
        }
    }
    ss << (cnt ? "\n" : "");
    return ss.str();
}

template <typename Graph>
string compact_dot(const Graph& g, bool directed = false) {
    static const char* header[] = {"strict graph", "strict digraph"};
    static const char* arrow[] = {" -- ", " -> "};
    stringstream ss;
    ss << header[directed] << " {\n";
    const int w = int(log10(len(g))) + 2;
    int cnt = 0;
    for (int u = 0; u < len(g); u++) {
        for (int v : g.adj[u]) {
            if (directed || u <= v) {
                ss << (cnt ? "  " : "    ") << setw(w) << u << arrow[directed];
                ss << setw(w) << v << " ;";
                if (++cnt == 8)
                    ss << "\n", cnt = 0;
            }
        }
    }
    ss << (cnt ? "\n}\n" : "}\n");
    return ss.str();
}

template <typename Graph>
string dimacs(const Graph& g) {
    stringstream ss;
    ss << "p edge " << g.V << " " << g.E << "\n";
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            if (u <= v) {
                ss << "e " << setw(2) << u << " " << setw(2) << v << "\n";
            }
        }
    }
    return ss.str();
}

template <typename Graph>
string to_human(const Graph& g) {
    stringstream ss;
    ss << "V=" << g.V << ", E=" << g.E << '\n';
    ss << adj_matrix(g) << adj_list(g);
    return ss.str();
}

string to_human(const bipartite_graph& g) {
    stringstream ss;
    ss << "U=" << g.U << ", V=" << g.V << ", E=" << g.E << '\n';
    ss << adj_matrix(g) << adj_list(g);
    return ss.str();
}

template <typename Graph>
string to_simple(const Graph& g, const string& more, bool directed = false) {
    stringstream ss;
    ss << g.V << ' ' << g.E << ' ' << more << '\n';
    ss << compact_simple(g, directed);
    return ss.str();
}

template <typename Graph>
string to_dot(const Graph& g, bool directed = false) {
    return compact_dot(g, directed);
}

template <typename Graph>
string to_dimacs(const Graph& g) {
    return dimacs(g);
}

template <typename Graph>
Graph read_simple(string& more, istream& in = cin) {
    int V, E;
    in >> V >> E;
    getline(in, more);
    Graph g(V);
    for (int e = 0; e < E; e++) {
        int u, v;
        char c;
        in >> u >> c >> v;
        assert(c == ',');
        g.add(u, v);
    }
    return g;
}

template <typename Graph>
string to_string(const Graph& g) {
    return to_human(g);
}

ostream& operator<<(ostream& out, const graph& g) { return out << to_human(g); }
ostream& operator<<(ostream& out, const digraph& g) { return out << to_human(g); }
ostream& operator<<(ostream& out, const bipartite_graph& g) { return out << to_human(g); }

#endif // GRAPH_FORMATS_HPP
