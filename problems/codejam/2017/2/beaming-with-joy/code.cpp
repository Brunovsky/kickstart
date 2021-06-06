#include <bits/stdc++.h>

using namespace std;

// *****

struct twosat_scc {
    int N, C = 0; // number of variables, half the number of nodes
    vector<unordered_set<int>> adj;
    vector<int> assignment, cmap;

    twosat_scc(int N = 0) : N(N), adj(2 * N) {}

    int add_var() { return adj.resize(2 * N + 2), N++; }

    void either(int u, int v) {
        assert(0 <= u && u < 2 * N && 0 <= v && v < 2 * N);
        adj[u ^ 1].insert(v);
        adj[v ^ 1].insert(u);
    }

    void set(int u) { adj[u ^ 1].insert(u); }

    vector<int> index, lowlink;
    vector<bool> onstack;
    stack<int> S;
    int depth;

    void tarjan(int u) {
        index[u] = depth;
        lowlink[u] = depth;
        depth++;
        S.push(u);
        onstack[u] = true;

        for (int v : adj[u]) {
            if (!index[v]) {
                tarjan(v);
                lowlink[u] = min(lowlink[u], lowlink[v]);
            } else if (onstack[v]) {
                lowlink[u] = min(lowlink[u], index[v]);
            }
        }

        if (index[u] == lowlink[u]) {
            int v, c = C++;
            do {
                v = S.top(), S.pop();
                onstack[v] = false;
                cmap[v] = c;
                if (assignment[v >> 1] == -1)
                    assignment[v >> 1] = v & 1;
            } while (u != v);
        }
    }

    bool solve() {
        assignment.assign(N, -1);
        cmap.assign(2 * N, 0);
        index.assign(2 * N, 0);
        lowlink.assign(2 * N, 0);
        onstack.assign(2 * N, false);
        S = stack<int>();
        depth = 1, C = 0;

        for (int u = 0; u < 2 * N; u++) {
            if (!index[u]) {
                tarjan(u);
            }
        }
        for (int u = 0; u < N; u++) {
            assert(assignment[u] >= 0);
            if (cmap[2 * u] == cmap[2 * u + 1]) {
                return false;
            }
        }
        return true;
    }
};

#define IS_NW_MIRROR(c) (c == '\\')
#define IS_NE_MIRROR(c) (c == '/')
#define IS_WALL(c)      (c == '#')
#define IS_BEAMER(c)    (c == '-' || c == '|')
#define IS_EMPTY(c)     (c == '.')

#define MAXR 60

int R, C, B;
char house[MAXR][MAXR];
int id[MAXR][MAXR][2];

bool visit_beam(array<int, 2> beam, int beam_dir, int node) {
    // directions: {! group: up=0, down=1} {- group: right=2, left=3}
    static constexpr int dd[4][2] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
    static constexpr int nw_map[4] = {3, 2, 1, 0}, ne_map[4] = {2, 3, 0, 1};

    vector<array<int, 3>> empty_cells;

    for (int dir : {beam_dir, beam_dir + 1}) {
        auto [r, c] = beam;
        do {
            r += dd[dir][0], c += dd[dir][1];

            if (IS_EMPTY(house[r][c])) {
                empty_cells.push_back({r, c, dir >> 1});
            }
            if (IS_BEAMER(house[r][c])) {
                return false;
            }
            if (IS_NW_MIRROR(house[r][c])) {
                dir = nw_map[dir];
            }
            if (IS_NE_MIRROR(house[r][c])) {
                dir = ne_map[dir];
            }
        } while (!IS_WALL(house[r][c]));
    }

    for (auto [r, c, d] : empty_cells) {
        assert(id[r][c][d] == -1);
        id[r][c][d] = node;
    }

    return true;
}

auto solve() {
    cin >> R >> C >> ws;
    memset(house, '#', sizeof(house));
    memset(id, 0xff, sizeof(id));

    vector<array<int, 2>> beamers;

    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            cin >> house[r][c] >> ws;
            if (IS_BEAMER(house[r][c]))
                beamers.push_back({r, c});
        }
    }
    B = beamers.size();

    twosat_scc sat(B);

    for (int b = 0; b < B; b++) {
        // is beaming N/S okay? | (beam 2b)
        bool ok0 = visit_beam(beamers[b], 0, 2 * b);
        // is beaming W/E okay? - (beam 2b+1)
        bool ok1 = visit_beam(beamers[b], 2, 2 * b + 1);

        if (!ok0 && !ok1)
            return false;
        if (ok0 != ok1)
            sat.set(2 * b + ok1);
    }

    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            if (!IS_EMPTY(house[r][c]))
                continue;

            auto [id0, id1] = id[r][c];

            if (id0 == -1 && id1 == -1) {
                return false;
            } else if (id0 == -1 || id1 == -1 || id0 == id1) {
                int beam = id0 == -1 ? id1 : id0;
                sat.set(beam);
            } else {
                sat.either(id0, id1);
            }
        }
    }

    if (!sat.solve())
        return false;

    for (int b = 0; b < B; b++) {
        auto [r, c] = beamers[b];
        house[r][c] = sat.assignment[b] ? '-' : '|';
    }
    return true;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        if (solve()) {
            cout << "POSSIBLE\n";
            for (int r = 1; r <= R; r++) {
                for (int c = 1; c <= C; c++) {
                    cout << house[r][c];
                }
                cout << '\n';
            }
        } else {
            cout << "IMPOSSIBLE\n";
        }
    }
    return 0;
}
