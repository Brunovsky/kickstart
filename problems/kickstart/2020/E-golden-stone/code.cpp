#include <bits/stdc++.h>

using namespace std;

// *****

static constexpr long MAX_COST = 1'000'000'000'000L;

int N, M, S, R;

// output stone of each recipe and size of each recipe
vector<int> O, K;

// stones in each location
vector<unordered_set<int>> stones;

// where to find each stone
vector<unordered_set<int>> where;

// stone recipes
vector<vector<int>> recipe;
vector<vector<int>> recipe_by_size;

// all pairs shortest paths
vector<vector<int>> adj;

// basic[s][u]: cost of supplying 1 stone s to node u.
vector<vector<long>> basic;

// cost(r, u): cost of producing recipe r from scratch in node u.
long cost(int i, int u) {
    long total = 0;
    for (int s : recipe[i])
        total += basic[s][u];
    return total;
}

void debug() {
    cout << "\n\n";
    for (int s = 0; s < S; s++) {
        for (int u = 0; u < N; u++) {
            string str = basic[s][u] >= 1000 ? "++"s : to_string(basic[s][u]);
            cout << setw(3) << str << ' ';
        }
        cout << '\n';
    }
    cout << "\n\n";
}

int compute_round(int size) {
    bool changed;
    int rounds = 0;
    do {
        changed = false;
        for (int i : recipe_by_size[size]) {
            for (int u = 0; u < N; u++) {
                long c = cost(i, u);
                if (basic[O[i]][u] > c) {
                    basic[O[i]][u] = c;
                    changed = true;
                }
            }
            printf("after recipe %d\n", i);
            debug();
        }
        rounds++;
    } while (changed);
    return rounds;
}

auto solve() {
    cin >> N >> M >> S >> R;
    O.assign(R, 0);
    K.assign(R, 0);
    stones.assign(N, {});
    where.assign(S, {});
    recipe.assign(R, {});
    recipe_by_size.clear();
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
            stones[i].insert(s);
            where[s].insert(i);
        }
    }
    for (int i = 0; i < R; i++) {
        cin >> K[i];
        maxK = max(maxK, K[i]);
        recipe_by_size.resize(maxK + 1, {});
        recipe_by_size[K[i]].push_back(i);
        for (int j = 0; j < K[i]; j++) {
            int s;
            cin >> s, s--;
            recipe[i].push_back(s);
        }
        cin >> O[i], O[i]--;
    }

    for (int s = 0; s < S; s++) {
        vector<int> bfs, next_bfs;
        vector<bool> vis(N, false);
        for (int u : where[s]) {
            bfs.push_back(u), vis[u] = true, basic[s][u] = 0;
        }
        int dist = 0;

        while (!bfs.empty()) {
            dist++;
            for (int u : bfs)
                for (int v : adj[u])
                    if (!vis[v])
                        next_bfs.push_back(v), vis[v] = true, basic[s][v] = dist;
            bfs = move(next_bfs);
            next_bfs.clear();
        }
    }

    int rounds;
    do {
        rounds = 0;
        for (int k = 1; k <= maxK; k++) {
            rounds += compute_round(k);
        }
        debug();
        fprintf(stderr, "rounds: %d\n", rounds);
    } while (rounds > maxK);

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
