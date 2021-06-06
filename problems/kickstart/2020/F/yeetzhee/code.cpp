#include <bits/stdc++.h>

using namespace std;

// *****

using part_t = vector<int8_t>;

struct hasher {
    size_t operator()(const part_t& vec) const noexcept {
        hash<int> hasher;
        size_t seed = vec.size();
        for (int n : vec) {
            seed ^= hasher(n) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

int lenrange(const part_t& b, int i) {
    int j = i + 1, S = b.size();
    while (j < S && b[i] == b[j])
        j++;
    return j - i;
}

auto solve() {
    int N, M, K;
    cin >> N >> M >> K;
    part_t b(K);
    for (int i = 0, n; i < K; i++)
        cin >> n, b[i] = n;
    sort(begin(b), end(b));

    // rep[i]: how many sets of coins have >=i coins. rep[1]=K.
    vector<int> rep(N + 2, 0);
    for (int i = 0; i < K; i += rep[b[i]]) {
        rep[b[i]] = lenrange(b, i);
    }
    for (int i = N - 1; i >= 0; i--) {
        rep[i] += rep[i + 1];
    }
    assert(rep[1] == K);

    unordered_map<part_t, int, hasher> idpart;
    vector<part_t> parts;
    vector<int> coins;
    vector<vector<pair<int, int>>> children; // <v, coins>
    int Q = 1;

    idpart[{1}] = 0;
    parts.push_back({1});

    for (int u = 0; u < Q; u++) {
        auto part = parts[u];
        int i = 0, S = part.size();
        coins.push_back(0);
        children.push_back({});

        // roll a repeated number, one set size increases by 1
        while (i < S) {
            int len = lenrange(part, i);
            int j = i + len;
            int repetitions = S - j + 1; // sets of size >=dice+1 after increase
            int dice = part[i];

            if (repetitions <= rep[dice + 1]) {
                part[j - 1]++;
                if (!idpart.count(part)) {
                    idpart[part] = Q++;
                    parts.push_back(part);
                }
                int v = idpart.at(part);
                children[u].push_back({v, len});
                part[j - 1]--;
                coins[u] += len;
            }

            i = j;
        }

        // roll a new number, new set of size 1
        if (S < K) {
            part.insert(begin(part), 1);
            if (!idpart.count(part)) {
                idpart[part] = Q++;
                parts.push_back(part);
            }
            int v = idpart.at(part);
            children[u].push_back({v, M - S});
            part.erase(begin(part));
            coins[u] += M - S;
        }
    }

    assert(parts.back() == b);

    vector<double> rolls(Q, 0.0);
    rolls[Q - 1] = 0.0; // no rolls on the last position

    // can roll coins[u] coins out of M, expected number of rolls to get one of
    // these coins is M/coins[u]. the probability of rolling the path u->v is
    // len(v)/coins[u] for each v.
    for (int u = Q - 2; u >= 0; u--) {
        double expected = 0.0;
        for (auto each : children[u]) {
            int v = each.first, len = each.second;
            expected += len * rolls[v];
        }
        rolls[u] = (M + expected) / coins[u];
    }

    return 1.0 + rolls[0];
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    cout << fixed << setprecision(7);
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
