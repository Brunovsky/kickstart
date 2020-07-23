#include <bits/stdc++.h>

using namespace std;

// *****

struct bundle_t {
    int score = 0;
    int excess = 0;
};

using iter_t = vector<string>::const_iterator;

#define DONE '\0'

char at(iter_t iter, size_t i) {
    return i == iter->size() ? DONE : (*iter)[i];
}

bundle_t make_bundle(iter_t begin, iter_t end, size_t i, int K) {
    bundle_t bundle;

    while (begin != end) {
        iter_t it = begin;
        char c = at(it, i);
        int moved = 0;
        while (it != end && at(it, i) == c) {
            ++it, ++moved;
        }
        if (moved < K) {
            bundle.excess += moved;
        } else if (c == DONE) {
            bundle.score += i * (moved / K);
            bundle.excess += (moved % K);
        } else {
            bundle_t recursed = make_bundle(begin, it, i + 1, K);
            bundle.score += recursed.score;
            bundle.excess += recursed.excess;
        }
        begin = it;
    }

    bundle.score += i * (bundle.excess / K);
    bundle.excess %= K;
    return bundle;
}

auto solve() {
    int N, K;
    cin >> N >> K;
    vector<string> s(N);
    for (int i = 0; i < N; ++i) {
        cin >> s[i];
    }
    sort(begin(s), end(s));
    return make_bundle(begin(s), end(s), 0, K).score;
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
