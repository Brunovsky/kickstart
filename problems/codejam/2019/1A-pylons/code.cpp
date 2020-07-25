#include <bits/stdc++.h>

using namespace std;

// *****

struct place_t {
    int r, c;
};

bool operator<(place_t lhs, place_t rhs) {
    return make_pair(lhs.r, lhs.c) < make_pair(rhs.r, rhs.c);
}

#define MAXN 21

vector<place_t> solutions[MAXN][MAXN];

vector<place_t> sol3x4 = {
    {2, 3}, {3, 1}, {2, 4}, {1, 2}, {3, 3}, {1, 4}, //
    {2, 2}, {3, 4}, {2, 1}, {1, 3}, {3, 2}, {1, 1}, //
};
vector<place_t> sol2x5 = {
    {1, 5}, {2, 3}, {1, 1}, {2, 4}, {1, 2}, {2, 5}, {1, 3}, {2, 1}, {1, 4}, {2, 2},
};

bool ok(place_t a, place_t b) {
    return a.r != b.r && a.c != b.c && //
           a.r + a.c != b.r + b.c && a.r - a.c != b.r - b.c;
}

void check(const vector<place_t>& vec) {
    for (uint i = 0; i + 1 < vec.size(); i++) {
        assert(ok(vec[i], vec[i + 1]));
    }
}

auto flip(const vector<place_t>& vec) {
    auto flipped = vec;
    for (uint i = 0; i < flipped.size(); i++) {
        swap(flipped[i].r, flipped[i].c);
    }
    check(flipped);
    return flipped;
}

void setup() {
    check(sol3x4);
    check(sol2x5);

    solutions[3][4] = sol3x4;
    solutions[4][3] = flip(sol3x4);

    reverse(begin(sol2x5), end(sol2x5));
    solutions[2][5] = sol2x5;
    solutions[5][2] = flip(sol2x5);

    for (int C = 6; C < MAXN; C++) {
        auto& solution = solutions[2][C];
        auto& prev = solutions[2][C - 1];

        solution.resize(2 * C);
        solution[0] = {2, C};
        for (int i = 1; i < 2 * C - 1; i++) {
            solution[i] = {3 - prev[i - 1].r, C - prev[i - 1].c};
        }
        solution[2 * C - 1] = {1, C};

        check(solution);

        solutions[C][2] = flip(solution);
    }

    for (int R = 3; R < MAXN; R++) {
        for (int C = R; C < MAXN; C++) {
            const auto& prev = solutions[R][C - 1];
            auto& solution = solutions[R][C];
            if (prev.empty()) {
                continue;
            }

            set<place_t> places;
            for (int r = 1; r <= R; r++) {
                places.insert({r, C});
            }

            uint M = prev.size();
            for (uint i = 0; i < M; i++) {
                solution.push_back(prev[i]);
                for (place_t p : places) {
                    if (ok(p, prev[i]) && (i + 1 == M || ok(p, prev[i + 1]))) {
                        solution.push_back(p);
                        places.erase(p);
                        break;
                    }
                }
            }
            // fingers crossed :)
            assert(places.empty());

            solutions[C][R] = flip(solution);
        }
    }
}

auto solve() {
    int R, C;
    cin >> R >> C;
    const auto& S = solutions[R][C];
    if (S.empty()) {
        return "IMPOSSIBLE"s;
    }
    string ans = "POSSIBLE";
    for (int i = 0; i < R * C; i++) {
        ans += '\n' + to_string(S[i].r) + ' ' + to_string(S[i].c);
    }
    return ans;
}

// *****

int main() {
    setup();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
