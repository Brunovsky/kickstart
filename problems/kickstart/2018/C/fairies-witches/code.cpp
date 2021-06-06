#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 15

int N;
int M[MAXN][MAXN];

struct edge_t {
    int i, j;
};

bool operator<(edge_t lhs, edge_t rhs) noexcept {
    return M[lhs.i][lhs.j] < M[rhs.i][rhs.j];
}

bool connected(edge_t lhs, edge_t rhs) {
    return lhs.i == rhs.i || lhs.j == rhs.i || lhs.i == rhs.j || lhs.j == rhs.j;
}

int dfs(vector<edge_t> edges, edge_t e0, int minsum) {
    auto newbegin = remove_if(begin(edges), end(edges), [e0](edge_t edge) {
        return connected(e0, edge);
    });
    edges.erase(newbegin, end(edges));

    int solutions = 0;

    while (!edges.empty()) {
        edge_t e = edges.back();
        edges.pop_back();
        solutions += M[e.i][e.j] >= minsum;
        int newminsum = max(0, minsum - M[e.i][e.j]);
        solutions += dfs(edges, e, newminsum);
    }

    return solutions;
}

auto solve() {
    cin >> N;
    memset(M, 0, sizeof(M));
    vector<edge_t> edges;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cin >> M[i][j];
            if (i < j && M[i][j] > 0) {
                edges.push_back({i, j});
            }
        }
    }
    sort(begin(edges), end(edges));

    int solution = 0;
    while (!edges.empty()) {
        edge_t e = edges.back();
        edges.pop_back();
        solution += dfs(edges, e, M[e.i][e.j] + 1);
    }
    return solution;
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
