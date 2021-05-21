#include <bits/stdc++.h>

using namespace std;

// *****

// (2N choose N) and (3N choose N)
constexpr int choose2N[] = {1, 2, 6, 20, 70, 252};
constexpr int choose3N[] = {1, 3, 15, 84, 495, 3003};

#define MAXSZ  16
#define MAXSUM 5'000'001
#define MAXID  (choose3N[5] + 1)

using SumMask = pair<int, int>;

int SUP1MASK;
int SUP2MASK;
int NMASK;

// When it doubt, it's a fenwick tree. Even if you doubt it's a fenwick tree!
int N, ids_A, ids_B;
int fenwick2[MAXID];
int fenwick23[MAXID][MAXID];
int beaten2[MAXID][MAXID];
int beaten3[MAXID][MAXID];

int Asum, Bsum;
int A[MAXSZ];    // Bala
int B[MAXSZ];    // Bahu
int Aid[MAXSUM]; // Ai -> id A
int Bid[MAXSUM]; // Bi -> id B
int BtoA[MAXID]; // id B --> largest id A such that Ai < Bi

inline void next_lexicographical_mask(int &v) {
    int t = v | (v - 1);
    v = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
}

int sum2(int i) {
    int sum = 0;
    while (i > 0) {
        sum += fenwick2[i];
        i -= i & -i;
    }
    return sum;
}

void add2(int i) {
    while (i <= ids_A) {
        fenwick2[i]++;
        i += i & -i;
    }
}

int sum23(int i, int j) {
    int sum = 0;
    while (i > 0) {
        int k = j;
        while (k > 0) {
            sum += fenwick23[i][k];
            k -= k & -k;
        }
        i -= i & -i;
    }
    return sum;
}

void add23(int i, int j) {
    while (i <= ids_A) {
        int k = j;
        while (k <= ids_A) {
            fenwick23[i][k]++;
            k += k & -k;
        }
        i += i & -i;
    }
}

int sum_mask(int mask, int V[]) {
    int i = 0, sum = 0;
    while (mask) {
        if (mask & 1) {
            sum += V[i];
        }
        i++, mask >>= 1;
    }
    return sum;
}

// V is A or B
vector<SumMask> makesums(int V[], int Vid[], int &ids_V) {
    vector<SumMask> summasks;
    summasks.reserve(choose3N[N] + 1);

    int mask = NMASK;
    do {
        int sum = sum_mask(mask, V);
        summasks.push_back({sum, mask});
        next_lexicographical_mask(mask);
    } while (mask < SUP1MASK);
    sort(begin(summasks), end(summasks));

    int i = 0, S = summasks.size();
    ids_V = 0;
    while (i < S) {
        int Vsum = summasks[i].first;
        int j = i + 1;
        while (j < S && summasks[j].first == Vsum) {
            ++j;
        }
        Vid[Vsum] = ++ids_V;
        i = j;
    }
    Vid[MAXSUM - 1] = ids_V + 1;

    return summasks;
}

void remaining_cards(int mask, int V[], int Vremaining[]) {
    for (int i = 0, j = 0; i < 3 * N; i++) {
        if (!(mask & (1 << i))) {
            Vremaining[j++] = V[i];
        }
    }
}

void mapids(vector<SumMask> &asum, const vector<SumMask> &bsum) {
    int a = 0;
    int b = 0;
    asum.push_back({MAXSUM - 1, 0});
    while (b < choose3N[N]) {
        while (a < choose3N[N] && asum[a].first < bsum[b].first) {
            a++;
        }
        int aid = Aid[asum[a].first];
        int bid = Bid[bsum[b].first];
        BtoA[bid] = aid - 1;
        b++;
    }
    asum.pop_back();
}

void read() {
    ids_A = ids_B = Asum = Bsum = 0;
    memset(fenwick2, 0, sizeof(fenwick2));
    memset(fenwick23, 0, sizeof(fenwick23));
    memset(beaten2, 0, sizeof(beaten2));
    memset(beaten3, 0, sizeof(beaten3));
    memset(A, 0, sizeof(A));
    memset(B, 0, sizeof(B));
    memset(Aid, 0, sizeof(Aid));
    memset(Bid, 0, sizeof(Bid));
    memset(BtoA, 0, sizeof(BtoA));
    cin >> N >> ws;
    for (int i = 0; i < 3 * N; i++) {
        cin >> B[i] >> ws;
    }
    for (int i = 0; i < 3 * N; i++) {
        cin >> A[i] >> ws;
    }
    Asum = accumulate(begin(A), end(A), 0);
    Bsum = accumulate(begin(B), end(B), 0);
}

auto solve() {
    read();
    SUP1MASK = 1 << (3 * N);
    SUP2MASK = 1 << (2 * N);
    NMASK = (1 << N) - 1;

    auto asum = makesums(A, Aid, ids_A);
    auto bsum = makesums(B, Bid, ids_B);
    mapids(asum, bsum);

    int Aremaining[MAXSZ] = {};
    int Bremaining[MAXSZ] = {};
    int a = 0;

    for (SumMask b1sm : bsum) {
        int B1 = b1sm.first;
        int b1mask = b1sm.second;
        remaining_cards(b1mask, B, Bremaining);

        // advance A1 until it exceeds B1
        while (a < choose3N[N] && asum[a].first < B1) {
            int A1 = asum[a].first;
            int a1mask = asum[a].second;
            remaining_cards(a1mask, A, Aremaining);
            a++;

            int a2mask = NMASK;
            do {
                int A2 = sum_mask(a2mask, Aremaining);
                int A3 = Asum - (A1 + A2);
                next_lexicographical_mask(a2mask);
                int a2 = Aid[A2];
                int a3 = Aid[A3];

                add2(a2);
                add23(a2, a3);
            } while (a2mask < SUP2MASK);
        }

        int b2mask = NMASK;
        do {
            int B2 = sum_mask(b2mask, Bremaining);
            int B3 = Bsum - (B1 + B2);
            next_lexicographical_mask(b2mask);
            int b1 = Bid[B1];
            int b2 = Bid[B2];
            int b3 = Bid[B3];

            // WLOG B1 >= B2 >= B3
            if (B1 >= B2) {
                beaten2[b1][b2] = sum2(BtoA[b2]);
                if (B2 >= B3) {
                    beaten3[b2][b3] = sum23(BtoA[b2], BtoA[b3]);
                }
            }
        } while (b2mask < SUP2MASK);
    }

    int max_beaten = 0;

    for (SumMask b1sm : bsum) {
        int B1 = b1sm.first;
        int b1mask = b1sm.second;
        remaining_cards(b1mask, B, Bremaining);

        int b2mask = NMASK;
        do {
            int B2 = sum_mask(b2mask, Bremaining);
            int B3 = Bsum - (B1 + B2);
            next_lexicographical_mask(b2mask);
            int b1 = Bid[B1];
            int b2 = Bid[B2];
            int b3 = Bid[B3];

            // WLOG B1 >= B2 >= B3
            if (B1 >= B2 && B2 >= B3) {
                int size_12 = beaten2[b1][b2];
                int size_13 = beaten2[b1][b3];
                int size_23 = beaten2[b2][b3];
                int size_123 = beaten3[b2][b3];
                int beaten = (size_12 + size_13 + size_23) - 2 * size_123;
                max_beaten = max(max_beaten, beaten);
            }
        } while (b2mask < SUP2MASK);
    }

    return 1.0 * max_beaten / (choose3N[N] * choose2N[N]);
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(8);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
