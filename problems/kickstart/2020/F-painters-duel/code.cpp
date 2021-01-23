#include <bits/stdc++.h>

using namespace std;

// *****

using mask_t = unsigned long; // the entire state of the game, including A and B
using step_t = array<int, 2>; // the two cells where A and B are

struct hasher {
    size_t operator()(const tuple<mask_t, step_t, bool>& entry) const noexcept {
        auto [mask, step, aturn] = entry;
        return hash<size_t>{}((mask << 13) | (step[0] << 7) | (step[1] << 1) | aturn);
    }
};

#define BIT(n) (1UL << n)

unordered_map<tuple<mask_t, step_t, bool>, int, hasher> states[7];
int id[7][12];        // (r,c) to cell id
array<int, 2> at[36]; // id to (r,c)

void make_mappings() {
    int n = 0;
    for (int r = 1; r <= 6; r++) {
        for (int c = 1; c <= 2 * r - 1; c++) {
            at[n] = {r, c};
            id[r][c] = n++;
        }
    }
    assert(n == 36);
}

auto find_open_adjacent(int S, mask_t mask, int i) {
    auto [r, c] = at[i];
    vector<int> adjacent;
    if (c > 1)
        adjacent.push_back(id[r][c - 1]);
    if (c < 2 * r - 1)
        adjacent.push_back(id[r][c + 1]);
    if (r < S && (c & 1))
        adjacent.push_back(id[r + 1][c + 1]);
    if (!(c & 1))
        adjacent.push_back(id[r - 1][c - 1]);
    vector<int> available;
    for (int j : adjacent)
        if (~mask & BIT(j))
            available.push_back(j);
    return available;
}

auto recurse(int S, mask_t mask, step_t step, bool aturn = true) {
    if (states[S].count({mask, step, aturn})) {
        return states[S][{mask, step, aturn}];
    }
    auto amoves = find_open_adjacent(S, mask, step[0]);
    auto bmoves = find_open_adjacent(S, mask, step[1]);
    if (amoves.empty() && bmoves.empty()) {
        states[S][{mask, step, aturn}] = 0;
        return 0;
    } else if ((aturn && !amoves.empty()) || (!aturn && bmoves.empty())) {
        int best_score = INT_MIN;
        assert(!amoves.empty());
        for (int amove : amoves) {
            mask_t rmask = mask | BIT(amove);
            step_t rstep = {amove, step[1]};
            int rscore = recurse(S, rmask, rstep, false);
            best_score = max(best_score, rscore + 1);
        }
        states[S][{mask, step, true}] = best_score;
        return best_score;
    } else {
        int best_score = INT_MAX;
        assert(!bmoves.empty());
        for (int bmove : bmoves) {
            mask_t rmask = mask | BIT(bmove);
            step_t rstep = {step[0], bmove};
            int rscore = recurse(S, rmask, rstep, true);
            best_score = min(best_score, rscore - 1);
        }
        states[S][{mask, step, false}] = best_score;
        return best_score;
    }
}

auto solve() {
    int S, C, Ra, Ca, Rb, Cb;
    cin >> S >> Ra >> Ca >> Rb >> Cb >> C >> ws;
    int a = id[Ra][Ca], b = id[Rb][Cb];
    mask_t construction = 0;
    for (int i = 0; i < C; i++) {
        int r, c;
        cin >> r >> c >> ws;
        construction |= BIT(id[r][c]);
    }
    mask_t init = construction | BIT(a) | BIT(b);
    return recurse(S, init, {a, b});
}

// *****

int main() {
    make_mappings();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
