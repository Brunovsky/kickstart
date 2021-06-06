#include <bits/stdc++.h>

using namespace std;

// *****

static constexpr long MAX_COST = 1'000'000'000'000L;

int N, M, S, R;

// output stone of each recipe and size of each recipe
vector<int> O, K;

// where to find each stone
vector<vector<int>> where;

// stone recipes
vector<vector<int>> recipe;
vector<vector<int>> producers;
vector<vector<int>> targets;

// all pairs shortest paths
vector<vector<int>> adj;
vector<vector<int>> dist;

// basic[s][u]: cost of producing 1 stone s in node u
vector<vector<long>> basic;
vector<bool> updated;

// Complexity O(N(N + P(s)))
bool compute(int s) {
    bool changed = false;
    for (int u = 0; u < N; u++) {
        for (int i : producers[s]) {
            long c = 0;
            for (int x : recipe[i])
                c += basic[x][u];
            if (basic[s][u] > c) {
                basic[s][u] = c;
                changed = true;
            }
        }
        for (int v = 0; v < N; v++) {
            long c = basic[s][v] + dist[u][v];
            if (basic[s][u] > c) {
                basic[s][u] = c;
                changed = true;
            }
        }
    }
    updated[s] = changed;
    if (changed)
        for (int t : targets[s])
            updated[t] = true;
    return changed;
}

void compute_distances() {
    dist.assign(N, vector<int>(N));

    for (int u = 0; u < N; u++) {
        vector<int> bfs, next_bfs;
        vector<bool> vis(N, false);
        for (int v : adj[u]) {
            bfs.push_back(v), vis[v] = true, dist[u][v] = 1;
        }

        dist[u][u] = 0;
        int depth = 1;

        while (!bfs.empty()) {
            depth++;
            for (int w : bfs)
                for (int v : adj[w])
                    if (!vis[v])
                        next_bfs.push_back(v), vis[v] = true, dist[u][v] = depth;
            bfs = move(next_bfs);
            next_bfs.clear();
        }
    }
}

void init_basic() {
    for (int s = 0; s < S; s++)
        for (int u : where[s])
            basic[s][u] = 0;
}

auto solve() {
    cin >> N >> M >> S >> R;
    O.assign(R, 0);
    K.assign(R, 0);
    where.assign(S, {});
    recipe.assign(R, {});
    producers.assign(S, {});
    targets.assign(S, {});
    adj.assign(N, {});
    basic.assign(S, vector<long>(N, MAX_COST));
    int maxK = 0;

    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v, u--, v--;
        adj[u].push_back(v), adj[v].push_back(u);
    }
    for (int i = 0; i < N; i++) {
        int C, s;
        cin >> C;
        for (int j = 0; j < C; j++) {
            cin >> s, s--;
            where[s].push_back(i);
        }
    }
    for (int i = 0; i < R; i++) {
        cin >> K[i];
        maxK = max(maxK, K[i]);
        for (int j = 0; j < K[i]; j++) {
            int s;
            cin >> s, s--;
            recipe[i].push_back(s);
        }
        cin >> O[i], O[i]--;
        producers[O[i]].push_back(i);
        for (int s : recipe[i])
            targets[s].push_back(O[i]);
    }

    compute_distances();
    init_basic();

    updated.assign(S, true);

    int updates;
    do {
        updates = 0;
        for (int s = 0; s < S; s++)
            if (updated[s])
                updates += compute(s);
    } while (updates > 0);

    long min = *min_element(begin(basic[0]), end(basic[0]));
    return min < MAX_COST ? min : -1;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
