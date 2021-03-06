#include <bits/stdc++.h>

using namespace std;

// *****

int N, P, B;
int X[5002];
int chunks[5002][5002];
int shifts[5002][5002];
vector<int> indices;

void compute_chunk_shifts() {
    memset(chunks, 0, sizeof(chunks));
    memset(shifts, 0x7f, sizeof(shifts));

    for (int i = 1; i <= N; ++i) {
        int max_ij = 0;
        int min_ij = N + 1;
        int s = N + 1;
        int n = 0;
        for (int j = i; j <= N; ++j) {
            max_ij = max(max_ij, X[j]);
            min_ij = min(min_ij, X[j]);
            if (max_ij - min_ij == j - i) {
                int shift = min_ij - i;
                n = chunks[i][j] = (shift == s) ? n + 1 : 1;
                s = shifts[i][j] = shift;
            }
        }
    }
}

auto solve2() {
    int K = B;

    for (int b = 0; b < B; ++b) {
        int L = indices[b] + 1;
        int R = indices[b + 1];
        int L_chunks = chunks[1][L - 1]; // b
        int R_chunks = chunks[R + 1][N]; // B - (b + 1)

        // nothing to swap
        if (L == R) {
            continue;
        }

        vector<int> ls;
        vector<int> rs;

        for (int l = L; l + 1 <= R; ++l) {
            if (chunks[L][l] && shifts[L][l] == R - l) {
                ls.push_back(l);
            }
        }

        for (int r = R; r - 1 >= L; --r) {
            if (chunks[r][R] && shifts[r][R] == L - r) {
                rs.push_back(r);
            }
        }

        for (int l : ls) {
            for (int r : rs) {
                if (r <= l) {
                    break;
                }

                int M_chunks = chunks[l + 1][r - 1];
                int chunks = L_chunks + 1 + M_chunks + 1 + R_chunks;
                K = max(K, chunks);
            }
        }
    }

    return K;
}

auto solve3() {
    int K = B;

    // basically, swap X[L...] with X[...R]
    // then see how many partitions we can get in between
    for (int b = 0; b < B; ++b) {
        int L = indices[b] + 1;
        int R = indices[b + 1];
        int L_chunks = chunks[1][L - 1]; // b
        int R_chunks = chunks[R + 1][N]; // B - (b + 1)

        // nothing to swap
        if (L + 1 >= R) {
            continue;
        }

        vector<int> ls;
        vector<int> rs;

        for (int l = L; l + 2 <= R; ++l) {
            if (chunks[L][l] && shifts[L][l] >= 0) {
                ls.push_back(l);
            }
        }

        for (int r = R; r - 2 >= L; --r) {
            if (chunks[r][R] && shifts[r][R] <= 0) {
                rs.push_back(r);
            }
        }

        for (int l : ls) {
            for (int r : rs) {
                if (r <= l + 1) {
                    break;
                }

                int Sl = l - L + 1;
                int Sr = R - r + 1;
                int L_max = l + shifts[L][l];
                int R_min = r + shifts[r][R];

                // P = 2 case
                // swap X[L..l] and X[r..R]
                if (L_max == R && R_min == L) {
                    int M_chunks = chunks[l + 1][r - 1];
                    int k = L_chunks + M_chunks + R_chunks + 2;
                    K = max(K, k);
                }

                // P = 3 case, right shift
                // X[L..l]  l+1..m1-1  [m1..m2]  m2+1..r-1   [r..R]
                //    Sl       S1         Sm         S2        Sr
                // -->
                // X[r..R]  l+1..m1-1   [L..l]   m2+1..r-1  [m1..m2]
                //    Sr       S1         Sl         S2        Sm
                // Sl, Sr, Sm >= 1       S1, S2 >= 0
                // shifts[L][l]      == Sr + S1
                // shifts[l+1][m1-1] == Sr - Sl
                // shifts[m1][m2]    == Sr + S2
                // shifts[m2+1][r-1] == Sr - Sm
                else if (R_min == L) {
                    int S1 = shifts[L][l] - Sr;
                    int m1 = l + S1 + 1;

                    if (S1 && shifts[l + 1][m1 - 1] != Sr - Sl) {
                        continue;
                    }

                    for (int m2 = m1; m2 < r; ++m2) {
                        int S2 = r - 1 - m2;

                        if (shifts[m1][m2] != Sr + S2) {
                            continue;
                        }

                        int S1_chunks = chunks[l + 1][m1 - 1];
                        int S2_chunks = chunks[m2 + 1][r - 1];
                        int k = L_chunks + R_chunks + S1_chunks + S2_chunks + 3;
                        K = max(K, k);
                    }
                }

                // P = 3 case, left shift
                //  X[L..l]   l+1..m1-1  [m1..m2]  m2+1..r-1  [r..R]
                //     Sl        S1         Sm         S2       Sr
                // -->
                // X[m1..m2]  l+1..m1-1   [r..R]   m2+1..r-1  [L..l]
                //     Sm        S1         Sr         S2       Sl
                // Sl, Sr, Sm >= 1       S1, S2 >= 0
                // -shifts[l+1][m1-1] == Sl - Sm
                // -shifts[m1][m2]    == Sl + S1
                // -shifts[m2+1][r-1] == Sl - Sr
                // -shifts[r][R]      == Sl + S2
                else if (L_max == R) {
                    int S2 = -Sl - shifts[r][R];
                    int m2 = r - 1 - S2;

                    if (S2 && -shifts[m2 + 1][r - 1] != Sl - Sr) {
                        continue;
                    }

                    for (int m1 = m2; m1 > l; --m1) {
                        int S1 = m1 - 1 - l;

                        if (-shifts[m1][m2] != Sl + S1) {
                            continue;
                        }

                        int S1_chunks = chunks[l + 1][m1 - 1];
                        int S2_chunks = chunks[m2 + 1][r - 1];
                        int k = L_chunks + R_chunks + S1_chunks + S2_chunks + 3;
                        K = max(K, k);
                    }
                }
            }
        }
    }

    return K;
}

auto solve() {
    memset(X, 0, sizeof(X));
    indices.clear();

    cin >> N >> P >> ws;
    for (int i = 1; i <= N; ++i) {
        cin >> X[i];
    }

    compute_chunk_shifts();

    indices.push_back(0);
    for (int i = 1; i <= N; ++i) {
        if (shifts[1][i] == 0) {
            indices.push_back(i);
        }
    }
    B = indices.size() - 1;

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
