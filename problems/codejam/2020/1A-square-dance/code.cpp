#include <bits/stdc++.h>

using namespace std;

// clang-format off

// *****

struct node_t {
    int r, c;
};

bool operator<(node_t lhs, node_t rhs) {
    return make_pair(lhs.r, lhs.c) < make_pair(rhs.r, rhs.c);
}

// dancing links with vectors

#define EAST(n)       east[n.r][n.c]
#define WEST(n)       west[n.r][n.c]
#define NORTH(n)      north[n.r][n.c]
#define SOUTH(n)      south[n.r][n.c]

#define EAST_N(n)     (node_t{n.r, east[n.r][n.c]})
#define WEST_N(n)     (node_t{n.r, west[n.r][n.c]})
#define NORTH_N(n)    (node_t{north[n.r][n.c], n.c})
#define SOUTH_N(n)    (node_t{south[n.r][n.c], n.c})

#define IS(n)         !!SCORE(n)
#define HAS_EAST(n)   IS(EAST_N(n))
#define HAS_WEST(n)   IS(WEST_N(n))
#define HAS_NORTH(n)  IS(NORTH_N(n))
#define HAS_SOUTH(n)  IS(SOUTH_N(n))
#define CNT_ADJ(n)    (HAS_EAST(n) + HAS_WEST(n) + HAS_NORTH(n) + HAS_SOUTH(n))
#define HAS_ADJ(n)    CNT_ADJ(n)

#define SCORE(n)      score[n.r][n.c]
#define SCORE_ADJ(n)  (SCORE(EAST_N(n)) + SCORE(WEST_N(n)) + SCORE(NORTH_N(n)) + SCORE(SOUTH_N(n)))
#define AVG_ADJ(n)    (1.0 * SCORE_ADJ(n) / CNT_ADJ(n))
#define IS_LOSER(n)   (IS(n) && HAS_ADJ(n) && SCORE(n) < AVG_ADJ(n))

int R, C;
vector<vector<int>> east, west, north, south, score;

auto solve() {
    cin >> R >> C >> ws;
    east.assign(R + 2, vector<int>(C + 2, 0));
    west.assign(R + 2, vector<int>(C + 2, 0));
    north.assign(R + 2, vector<int>(C + 2, 0));
    south.assign(R + 2, vector<int>(C + 2, 0));
    score.assign(R + 2, vector<int>(C + 2, 0));

    long round_score = 0;
    vector<node_t> frontier;
    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            east[r][c] = c - 1;
            west[r][c] = c + 1;
            north[r][c] = r - 1;
            south[r][c] = r + 1;
            cin >> score[r][c] >> ws;
            round_score += score[r][c];
            frontier.push_back({r, c});
        }
    }

    long total_score = 0;
    int eliminated;

    do {
        eliminated = 0;
        total_score += round_score;

        vector<node_t> old_frontier;
        swap(old_frontier, frontier);
        set<node_t> losers;

        for (node_t n : old_frontier) {
            if (IS_LOSER(n)) {
                losers.insert(n);
            }
        }

        for (node_t n : losers) {
            EAST(WEST_N(n)) = EAST(n);
            WEST(EAST_N(n)) = WEST(n);
            NORTH(SOUTH_N(n)) = NORTH(n);
            SOUTH(NORTH_N(n)) = SOUTH(n);
            round_score -= SCORE(n);
            SCORE(n) = 0;
            if (HAS_EAST(n)) frontier.push_back(EAST_N(n));
            if (HAS_WEST(n)) frontier.push_back(WEST_N(n));
            if (HAS_NORTH(n)) frontier.push_back(NORTH_N(n));
            if (HAS_SOUTH(n)) frontier.push_back(SOUTH_N(n));
            eliminated++;
        }
    } while (eliminated);

    return total_score;
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
