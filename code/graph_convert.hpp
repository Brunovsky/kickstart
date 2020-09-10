#ifndef GRAPH_CONVERT_HPP
#define GRAPH_CONVERT_HPP

#include "graph.hpp"
#include "random.hpp"

// *****

cost_graph add_costs(const graph& g, longd cost) {
    cost_graph wg(g.V);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            wg.add(u, v, cost(mt));
    return wg;
}

cost_graph add_costs(const graph& g, long max_cost) {
    return add_costs(g, longd(0, max_cost));
}

cost_digraph add_costs(const digraph& g, longd cost) {
    cost_digraph wg(g.V);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            wg.add(u, v, cost(mt));
    return wg;
}

cost_digraph add_costs(const digraph& g, long max_cost) {
    return add_costs(g, longd(0, max_cost));
}

cost_flow_graph add_costs(const flow_graph& g, longd cost) {
    cost_flow_graph wg(g.V);
    for (int e = 0; e < g.E; e++) {
        int u = g.source[2 * e], v = g.target[2 * e], c = g.cap[2 * e];
        wg.add(u, v, c, cost(mt));
    }
    return wg;
}

cost_flow_graph add_costs(const flow_graph& g, long max_cost) {
    return add_costs(g, longd(0, max_cost));
}

cost_bipartite_graph add_costs(const bipartite_graph& g, longd cost) {
    cost_bipartite_graph wg(g.U, g.V);
    for (int u = 0; u < g.U; u++)
        for (int v : g.adj[u])
            wg.add(u, v, cost(mt));
    return wg;
}

cost_bipartite_graph add_costs(const bipartite_graph& g, long max_cost) {
    return add_costs(g, longd(0, max_cost));
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

cost_flow_graph add_caps_costs(const digraph& g, longd cap, longd cost) {
    cost_flow_graph fg(g.V);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            fg.add(u, v, cap(mt), cost(mt));
    return fg;
}

cost_flow_graph add_caps_costs(const digraph& g, long max_cap, long max_cost) {
    return add_caps_costs(g, longd(1, max_cap), longd(0, max_cost));
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

cost_digraph flowgrify(const cost_bipartite_graph& g, long sw, long tw) {
    int V = g.U + g.V + 2;
    cost_digraph fg(V);
    for (int u = 0; u < g.U; u++)
        fg.add(0, u + 1, sw);
    for (int v = 0; v < g.V; v++)
        fg.add(g.U + v + 1, V - 1, tw);
    for (int u = 0; u < g.U; u++)
        for (int e : g.adj[u])
            fg.add(u + 1, g.U + g.target[e] + 1, g.cost[e]);
    return fg;
}

#endif // GRAPH_CONVERT_HPP
