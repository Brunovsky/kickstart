#include <bits/stdc++.h>

using namespace std;

// *****

#define IS_OBSTACLE(x) (x <= -100000)
#define IS_TRAP(x)     (-100000 < x && x < 0)
#define IS_ROOM(x)     (0 <= x)

// https://stackoverflow.com/questions/1851134
inline void next_lexicographical_mask(int &v) {
    int t = v | (v - 1);
    v = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
}

#define MAXR     102
#define MAXC     102
#define MAXT     16
#define MAXN     (MAXR * MAXC)
#define MAXTMASK ((1 << MAXT) - 1)

int R, C, E;
int SR, SC, TR, TC;
int T;     // number of traps
int N;     // number of rooms
int TMASK; // global trap set mask

// traps
int tadj[MAXT];           // traps adjacent to trap #i (bitmask)
int strength[MAXT];       // strength of trap #i
vector<int> trooms[MAXT]; // rooms adjacent to trap #i

int tstart; // whether trap #i is adjacent to room_S (bitmask)
int texit;  // whether trap #i is adjacent to room_T (bitmask)

// rooms
int room_vis[MAXN];       // number of visits to room #i
int energy[MAXN];         // energy of room #i
vector<int> rtraps[MAXN]; // traps adjacent to room #i

// trap subsets
int unconstrained_energy[MAXTMASK]; // unconstrained solution for tmask
int constrained_energy[MAXTMASK];   // constrained solution for tmask
int treach[MAXTMASK];               // traps reachable from subset (bitmask)

// cave layout
int cave[MAXR][MAXC]; // the cave is surrounded by obstacles
int room_S;           // room S is in
int room_T;           // room T is in

void clean() {
    T = N = TMASK = 0;

    memset(strength, 0, sizeof(strength));
    memset(tadj, 0, sizeof(tadj));
    for (int i = 0; i < MAXT; i++) {
        trooms[i].clear();
    }

    tstart = texit = 0;

    memset(room_vis, 0, sizeof(room_vis));
    memset(energy, 0, sizeof(energy));
    for (int i = 0; i < MAXN; i++) {
        rtraps[i].clear();
    }

    memset(unconstrained_energy, 0, sizeof(unconstrained_energy));
    memset(constrained_energy, 0, sizeof(constrained_energy));
    memset(treach, 0, sizeof(treach));

    memset(cave, 0xfe, sizeof(cave)); // obstacles everywhere
    room_S = room_T = -1;
}

void analyze_cave() {
    int room = 0;
    int trap = 0;

    int ids[MAXR][MAXC] = {};
    bool vis[MAXR][MAXC] = {};
    memset(ids, 0xff, sizeof(ids));

    static constexpr int dd[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    auto floodfill_room = [&](int fr, int fc) {
        assert(IS_ROOM(cave[fr][fc]) && !vis[fr][fc]);
        ids[fr][fc] = room;

        list<array<int, 2>> frontier;
        frontier.push_back({fr, fc});

        while (!frontier.empty()) {
            auto cell = frontier.front();
            frontier.pop_front();
            int r = cell[0], c = cell[1];
            if (IS_ROOM(cave[r][c]) && !vis[r][c]) {
                for (auto d : dd) {
                    frontier.push_back({r + d[0], c + d[1]});
                }

                energy[room] += cave[r][c]; // add the energy in the cell to the room
                vis[r][c] = true;
                ids[r][c] = room;
            }
        }
        room++;
    };

    auto inspect_trap = [&](int r, int c) {
        assert(IS_TRAP(cave[r][c]) && !vis[r][c]);
        ids[r][c] = trap;
        strength[trap] = -cave[r][c];

        for (auto d : dd) {
            int dr = r + d[0], dc = c + d[1];
            if (IS_ROOM(cave[dr][dc])) {
                if (!vis[dr][dc]) {
                    floodfill_room(dr, dc);
                }
                int id = ids[dr][dc];
                trooms[trap].push_back(id);
                rtraps[id].push_back(trap);
            }
            // fancy, two traps adjacent to each other. ignore if we're first.
            if (IS_TRAP(cave[dr][dc]) && vis[dr][dc]) {
                int id = ids[dr][dc];
                tadj[trap] |= 1 << id;
                tadj[id] |= 1 << trap;
            }
        }
        vis[r][c] = true;
        trap++;
    };

    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            if (vis[r][c] || IS_OBSTACLE(cave[r][c])) {
                continue;
            }
            if (IS_TRAP(cave[r][c])) {
                inspect_trap(r, c);
            } else {
                floodfill_room(r, c);
            }
            assert(vis[r][c]);
        }
    }

    T = trap;
    N = room;
    TMASK = (1 << T) - 1;
    room_S = ids[SR][SC];
    room_T = ids[TR][TC];

    assert(IS_ROOM(cave[SR][SC]) && IS_ROOM(cave[TR][TC]));
    assert(room_S >= 0 && room_T >= 0);
}

void build_graph() {
    for (int room = 0; room < N; room++) {
        for (int trap1 : rtraps[room]) {
            for (int trap2 : rtraps[room]) {
                if (trap1 != trap2) {
                    tadj[trap1] |= 1 << trap2;
                    tadj[trap2] |= 1 << trap1;
                }
            }
        }
    }
    for (int trap : rtraps[room_S]) {
        tstart |= 1 << trap;
    }
    for (int trap : rtraps[room_T]) {
        texit |= 1 << trap;
    }
}

void visit_trap(int trap) {
    E -= strength[trap];
    for (int room : trooms[trap]) {
        if (!room_vis[room]++) {
            E += energy[room];
        }
    }
}

void unvisit_trap(int trap) {
    for (int room : trooms[trap]) {
        if (!--room_vis[room]) {
            E -= energy[room];
        }
    }
    E += strength[trap];
}

void compute_unconstrained(int tmask, int trap) {
    visit_trap(trap);

    int new_tmask = tmask | (1 << trap);
    unconstrained_energy[new_tmask] = E;
    treach[new_tmask] = (treach[tmask] | tadj[trap]) & ~new_tmask;

    for (int next_trap = trap + 1; next_trap < T; next_trap++) {
        compute_unconstrained(new_tmask, next_trap);
    }

    unvisit_trap(trap);
}

void compute_all_unconstrained() {
    for (int trap = 0; trap < T; trap++) {
        compute_unconstrained(0, trap);
    }
    treach[0] = tstart;
    unconstrained_energy[0] = E + energy[room_S];
}

void compute_constrained_energy(int tmask) {
    // it took me hours to find this bug, god help you:
    bool can_reach_T = tmask ? (tmask & texit) : room_S == room_T;
    int energy = can_reach_T ? unconstrained_energy[tmask] : -1;

    for (int trap = 0; trap < T; trap++) {
        if (treach[tmask] & (1 << trap)) {
            if (strength[trap] <= unconstrained_energy[tmask]) {
                energy = max(energy, constrained_energy[tmask | (1 << trap)]);
            }
        }
    }

    constrained_energy[tmask] = energy;
}

void constrained_dp() {
    for (int ntraps = T; ntraps > 0; ntraps--) {
        int tmask = (1 << ntraps) - 1;
        do {
            compute_constrained_energy(tmask);
            next_lexicographical_mask(tmask);
        } while (tmask <= TMASK);
    }
    compute_constrained_energy(0);
}

auto solve() {
    clean();
    cin >> R >> C >> E >> SR >> SC >> TR >> TC >> ws;
    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            cin >> cave[r][c] >> ws;
        }
    }
    analyze_cave();
    build_graph();
    compute_all_unconstrained();
    constrained_dp();
    return constrained_energy[0];
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
