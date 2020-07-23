#include <bits/stdc++.h>

using namespace std;

// *****

struct Edge {
    int node, cost;
};

int N, M, P;
vector<vector<Edge>> adj;
vector<int> cost;
vector<int> sums;

void dijkstra(int s) {
    for (int i = 1; i <= N; i++) {
        cost[i] = INT_MAX;
    }
    cost[s] = 0;
    vector<Edge> frontier{{s, 0}};
    while (!frontier.empty()) {
        auto curr = frontier.back();
        frontier.pop_back();

        int u = curr.node;
        if (curr.cost > cost[u]) {
            continue;
        }

        for (Edge edge : adj[u]) {
            int v = edge.node;
            if (cost[v] > cost[u] + edge.cost) {
                cost[v] = cost[u] + edge.cost;
                frontier.push_back({v, cost[v]});
            }
        }
    }
    for (int i = 1; i <= N; i++) {
        sums[i] += cost[i];
    }
}

auto solve() {
    cin >> N >> M >> P;
    adj.assign(N + 1, {});
    for (int i = 0; i < M; i++) {
        int u, v, cost;
        cin >> u >> v >> cost;
        adj[u].push_back({v, cost});
        adj[v].push_back({u, cost});
    }

    if (N == 2) {
        int cost = adj[1][0].cost;
        return 1.0 * cost * P;
    }

    cost.resize(N + 1, 0);
    sums.assign(N + 1, 0);

    dijkstra(1);

    int land_home_cost = 0;
    for (int i = 2; i <= N; i++) {
        land_home_cost += cost[i];
    }

    for (int s = 2; s <= N; s++) {
        dijkstra(s);
    }

    int land_land_cost = -land_home_cost;
    for (int i = 2; i <= N; i++) {
        land_land_cost += sums[i];
    }

    int home_land_cost = sums[1];

    double q = 1.0; // we are home with probability q
    double home_land_p = 1.0 * home_land_cost / (N - 1);
    double land_home_p = 1.0 * land_home_cost / (N - 1);
    double land_land_p = 1.0 * land_land_cost / ((N - 2) * (N - 1));
    int caught = 0;
    double probability = 0.0;

    do {
        double p1 = q;
        double p2 = (1 - q) / (N - 1);
        double p3 = (1 - q) * (N - 2) / (N - 1);
        double p1_p = p1 * home_land_p;
        double p2_p = p2 * land_home_p;
        double p3_p = p3 * land_land_p;
        probability += p1_p + p2_p + p3_p;
        q = p2;
    } while (++caught < P && abs(q * N - 1) > 1e-25);

    if (caught < P) {
        double p = 1 - 2 * q;
        double one = q * (home_land_p + land_home_p) + p * land_land_p;
        probability += (P - caught) * one;
    }

    return probability;
}

// let position 1 be the home position, and any other position be 'land'
// suppose you are home with probability q, and on land with probability 1 - q.
// - with probability q                you will move from home to land
// - with probability (1-q)/(N-1)      you will move from land to home
// - with probability (1-q)(N-2)/(N-1) you will move from land to land

// *****

int main() {
    unsigned T;
    cout << showpoint << fixed << setprecision(7);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
