#include <bits/stdc++.h>

using namespace std;

// *****

struct Cafe {
    int x, y;
};

bool operator<(Cafe lhs, Cafe rhs) {
    return tie(lhs.x, lhs.y) < tie(rhs.x, rhs.y);
}

#define LHS  0
#define RHS  1
#define MAXM 1'000'001UL

long N, M;
vector<Cafe> cafes;

int plain[2][MAXM];
int fenwick[2][MAXM];

inline int sum(int f, int i) {
    int sum = 0;
    while (i) {
        sum += fenwick[f][i];
        i -= i & -i;
    }
    return sum;
}

inline void add(int f, int i) {
    plain[f][i]++;
    while (i <= M) {
        fenwick[f][i]++;
        i += i & -i;
    }
}

inline void remove(int f, int i) {
    plain[f][i]--;
    while (i <= M) {
        fenwick[f][i]--;
        i += i & -i;
    }
}

void read() {
    long X0, Y0, A, B, C, D, E, F;
    cin >> N >> X0 >> Y0 >> A >> B >> C >> D >> E >> F >> M >> ws;
    cafes.resize(N);
    cafes[0] = {int(X0), int(Y0)};
    for (long i = 1; i < N; i++) {
        long x = (A * X0 + B * Y0 + C) % M;
        long y = (D * X0 + E * Y0 + F) % M;
        cafes[i] = {int(x), int(y)};
        X0 = x, Y0 = y;
    }
}

auto solve() {
    read();
    sort(begin(cafes), end(cafes));

    memset(plain, 0, sizeof(plain));
    memset(fenwick, 0, sizeof(fenwick));

    for (long i = 0; i < N; i++) {
        add(RHS, cafes[i].y + 1);
    }

    size_t exceptions = 0;
    int start = 0;
    while (start < N) {
        int end = start + 1;
        while (end < N && cafes[start].x == cafes[end].x) {
            end++;
        }

        for (long i = start; i < end; i++) {
            remove(RHS, cafes[i].y + 1);
        }

        size_t L = start;
        size_t R = N - end;

        for (long i = start; i < end; i++) {
            int s = cafes[i].y;
            size_t q0 = sum(LHS, s);
            size_t q1 = L - q0 - plain[LHS][s + 1];
            size_t q2 = sum(RHS, s);
            size_t q3 = R - q2 - plain[RHS][s + 1];
            exceptions += q0 * q3 + q1 * q2;
        }

        for (long i = start; i < end; i++) {
            add(LHS, cafes[i].y + 1);
        }
        start = end;
    }

    size_t choose3 = N * (N - 1) * (N - 2) / 6;
    return choose3 - exceptions;
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
