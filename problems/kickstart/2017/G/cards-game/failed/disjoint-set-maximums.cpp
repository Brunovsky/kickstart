#include <bits/stdc++.h>

using namespace std;

// *****

// LOL

auto solve() {
    uint N;
    cin >> N;
    vector<uint> A(N), B(N);
    for (uint i = 0; i < N; ++i)
        cin >> A[i];
    for (uint i = 0; i < N; ++i)
        cin >> B[i];

    vector<uint> M;
    uint64_t sum = 0;

    while (N > 1) {
        M.assign(N, UINT_MAX);
        for (uint i = 0; i + 1 < N; ++i) {
            for (uint j = i + 1; j < N; ++j) {
                uint lo = min(A[i] ^ B[j], A[j] ^ B[i]);
                M[i] = min(M[i], lo);
                M[j] = min(M[j], lo);
            }
        }

        unordered_set<uint> blacklist{0};
        for (uint i = 0; i < N; ++i) {
            if (M[i] < M[*blacklist.begin()])
                continue;
            if (M[i] > M[*blacklist.begin()])
                blacklist.clear();
            blacklist.insert(i);
        }

        uint c = M[*blacklist.begin()];
        uint s = blacklist.size();
        unordered_map<uint, uint> outside;

        // disjoint-set structure
        vector<uint> next(N);
        iota(next.begin(), next.end(), 0);

        auto find = [&](uint i) {
            while (i != next[i])
                i = next[i] = next[next[i]];
            return i;
        };

        auto join = [&](uint i, uint j) { return next[find(j)] = find(i); };

        for (uint i : blacklist) {
            outside[i] = UINT_MAX;
            for (uint j = 0; j < N; ++j) {
                if (blacklist.count(j))
                    continue;
                uint lo = min(A[i] ^ B[j], A[j] ^ B[j]);
                outside[i] = min(outside[i], lo);
                if (lo == c)
                    break;
            }
        }

        for (uint i : blacklist) {
            for (uint j : blacklist) {
                uint lo = min(A[i] ^ B[j], A[j] ^ B[j]);
                if (lo == c)
                    join(i, j);
            }
        }

        unordered_map<uint, uint> best;
        for (uint i : blacklist) {
            uint root = find(i);
            if (best.count(root) == 0)
                best.emplace(root, M[root]);
            best[root] = min(best[root], M[i]);
        }

        uint k = best.size(), maximum = UINT_MAX;
        sum += c * (s - k);
        for (auto sets : best) {
            sum += sets.second;
            maximum = min(maximum, sets.second);
        }
        {
            uint write = 0, read = 0;
            while (read < N) {
                if (!blacklist.count(read)) {
                    A[write] = A[read];
                    B[write] = B[read];
                    ++write;
                }
                ++read;
            }
            A.resize(write);
            B.resize(write);
            N = write;
        }
        if (N == 0) {
            sum -= maximum;
        }
    }

    return sum;
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
