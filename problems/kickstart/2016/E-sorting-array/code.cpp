#include <bits/stdc++.h>

using namespace std;

// *****

int N, P;
int X[5002];
int num_chunks[5002];
vector<int> indices;

auto solve2() {
    int B = indices.size() - 1;
    int K = B;

    // basically, swap X[L...] with X[...R]
    // then see how many partitions we can get in between
    for (int b = 0; b < B; ++b) {
        int L = indices[b] + 1;
        int R = indices[b + 1];
        int L_chunks = num_chunks[L - 1];
        int R_chunks = num_chunks[N] - num_chunks[R];

        // nothing to swap
        if (L == R)
            continue;

        int min_Ll = R + 1; // minimum in X[L..l]
        int max_rR = L - 1; // maximum in X[r..R]

        // we are gonna swap X[L..l] and X[r..R], ranges inclusive
        vector<int> ls;
        vector<int> rs;

        for (int l = L; l < R; ++l) {
            min_Ll = min(min_Ll, X[l]);
            if (min_Ll == R - (l - L))
                ls.push_back(l);
        }
        for (int r = R; r > L; --r) {
            max_rR = max(max_rR, X[r]);
            if (max_rR == L + (R - r))
                rs.push_back(r);
        }

        for (int l : ls) {
            for (int r : rs) {
                if (r <= l)
                    continue;
                // swap X[L..l] and X[r..R]
                // how many chunks can we split the middle X[l+1..r-1] into?
                int shift = (R - r) - (l - L); // middle shift after swap
                int max_lr = 0;                // maximum in X[l+1..i]
                int M_chunks = 0;
                for (int M = l + 1; M < r; ++M) {
                    max_lr = max(max_lr, X[M]);
                    if (max_lr == M + shift)
                        ++M_chunks;
                }
                int chunks = L_chunks + 1 + M_chunks + 1 + R_chunks;
                K = max(K, chunks);
            }
        }
    }

    return K;
}

auto solve3() {
    return 0;
}

auto solve() {
    memset(X, 0, sizeof(X));
    memset(num_chunks, 0, sizeof(num_chunks));
    indices.clear();

    cin >> N >> P >> ws;
    for (int i = 1; i <= N; ++i)
        cin >> X[i];

    indices.push_back(0);

    int max_prefix = 0;
    for (int i = 1, k = 0; i <= N + 1; ++i) {
        max_prefix = max(max_prefix, X[i]);
        if (max_prefix == i) {
            num_chunks[i] = ++k;
            indices.push_back(i);
        }
    }

    return P == 2 ? solve2() : solve3();
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
