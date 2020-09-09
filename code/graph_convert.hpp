#ifndef GRAPH_CONVERT_HPP
#define GRAPH_CONVERT_HPP

#include "graph.hpp"
#include "random.hpp"

// *****

weight_graph add_weights(const graph& g, longd weight) {
    weight_graph wg(g.V);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            wg.add(u, v, weight(mt));
    return wg;
}

weight_graph add_weights(const graph& g, long max_weight) {
    return add_weights(g, longd(0, max_weight));
}

weight_digraph add_weights(const digraph& g, longd weight) {
    weight_digraph wg(g.V);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            wg.add(u, v, weight(mt));
    return wg;
}

weight_digraph add_weights(const digraph& g, long max_weight) {
    return add_weights(g, longd(0, max_weight));
}

weight_flow_graph add_weights(const flow_graph& g, longd weight) {
    weight_flow_graph wg(g.V);
    for (int e = 0; e < g.E; e++) {
        int u = g.source[2 * e], v = g.target[2 * e], c = g.cap[2 * e];
        wg.add(u, v, c, weight(mt));
    }
    return wg;
}

weight_flow_graph add_weights(const flow_graph& g, long max_weight) {
    return add_weights(g, longd(0, max_weight));
}

weight_bipartite_graph add_weights(const bipartite_graph& g, longd weight) {
    weight_bipartite_graph wg(g.U, g.V);
    for (int u = 0; u < g.U; u++)
        for (int v : g.adj[u])
            wg.add(u, v, weight(mt));
    return wg;
}

weight_bipartite_graph add_weights(const bipartite_graph& g, long max_weight) {
    return add_weights(g, longd(0, max_weight));
}

flow_graph add_caps(const digraph& g, longd cap) {
    flow_graph fg(g.V);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            fg.add(u, v, cap(mt));
    return fg;
}

flow_graph add_caps(const digraph& g, long max_cap) {
    return add_caps(g, longd(1, max_cap));
}

weight_flow_graph add_caps_weights(const digraph& g, longd cap, longd weight) {
    weight_flow_graph fg(g.V);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            fg.add(u, v, cap(mt), weight(mt));
    return fg;
}

weight_flow_graph add_caps_weights(const digraph& g, long max_cap, long max_weight) {
    return add_caps_weights(g, longd(1, max_cap), longd(0, max_weight));
}

digraph flowgrify(const bipartite_graph& g) {
    int V = g.U + g.V + 2;
    digraph fg(V);
    for (int u = 0; u < g.U; u++)
        fg.add(0, u + 1);
    for (int v = 0; v < g.V; v++)
        fg.add(g.U + v + 1, V - 1);
    for (int u = 0; u < g.U; u++)
        for (int v : g.adj[u])
            fg.add(u + 1, g.U + v + 1);
    return fg;
}

weight_digraph flowgrify(const weight_bipartite_graph& g, long sw, long tw) {
    int V = g.U + g.V + 2;
    weight_digraph fg(V);
    for (int u = 0; u < g.U; u++)
        fg.add(0, u + 1, sw);
    for (int v = 0; v < g.V; v++)
        fg.add(g.U + v + 1, V - 1, tw);
    for (int u = 0; u < g.U; u++)
        for (int e : g.adj[u])
            fg.add(u + 1, g.U + g.target[e] + 1, g.weight[e]);
    return fg;
}

#endif // GRAPH_CONVERT_HPP
