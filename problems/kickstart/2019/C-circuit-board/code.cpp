#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXS 300

int R, C, K;
int V[MAXS][MAXS];
int range[MAXS][MAXS];

inline bool out_of_bounds(const multiset<int> &span, int num) {
    if (span.empty()) {
        return false;
    }
    return num - *span.begin() > K || *span.rbegin() - num > K;
}

void rangify(int r) {
    int i = 0, j = 0;

    multiset<int> span;
    span.insert(V[r][i]);

    while (++j < C) {
        while (out_of_bounds(span, V[r][j])) {
            range[r][i] = span.size();
            span.erase(span.find(V[r][i++]));
        }
        span.insert(V[r][j]);
    }

    int S = span.size();
    while (i < C) {
        range[r][i++] = S--;
    }
    assert(i == C && S == 0);
}

int subcircuit(int r, int c) {
    int min_range = range[r][c];
    int rows = 1;
    int area = min_range;

    while (++r < R) {
        min_range = min(min_range, range[r][c]);
        area = max(area, min_range * ++rows);
    }

    return area;
}

auto solve() {
    cin >> R >> C >> K >> ws;
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            cin >> V[r][c] >> ws;
        }
    }

    for (int r = 0; r < R; ++r) {
        rangify(r);
    }

    int area = 1;
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            area = max(area, subcircuit(r, c));
        }
    }
    return area;
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
