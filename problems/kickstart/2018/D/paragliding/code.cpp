#include <bits/stdc++.h>

using namespace std;

// *****

struct Tower {
    int P, H;
};

struct Balloon {
    int X, Y;
};

bool operator<(Tower lhs, Tower rhs) {
    return lhs.P < rhs.P;
}

bool operator<(Balloon lhs, Balloon rhs) {
    return tie(lhs.X, lhs.Y) < tie(rhs.X, rhs.Y);
}

int N, K;

vector<Tower> towers;
vector<Balloon> balloons;

#define STEP(vec, i, Z, Z1, Z2, A, B, C, M)      \
    long long Z = (A * Z2 + B * Z1 + C) % M + 1; \
    vec[i].Z = Z;                                \
    Z1 = Z2;                                     \
    Z2 = Z

auto read() {
    long long A1, B1, C1, M1;
    long long A2, B2, C2, M2;
    long long A3, B3, C3, M3;
    long long A4, B4, C4, M4;
    long long P1, P2, H1, H2, X1, X2, Y1, Y2;
    cin >> N >> K >> ws;
    towers.resize(N);
    balloons.resize(K);

    cin >> P1 >> P2 >> A1 >> B1 >> C1 >> M1;
    cin >> H1 >> H2 >> A2 >> B2 >> C2 >> M2;
    cin >> X1 >> X2 >> A3 >> B3 >> C3 >> M3;
    cin >> Y1 >> Y2 >> A4 >> B4 >> C4 >> M4;
    towers[0].P = P1, towers[1].P = P2;
    towers[0].H = H1, towers[1].H = H2;
    balloons[0].X = X1, balloons[1].X = X2;
    balloons[0].Y = Y1, balloons[1].Y = Y2;
    for (int i = 2; i < N; i++) {
        STEP(towers, i, P, P1, P2, A1, B1, C1, M1);
        STEP(towers, i, H, H1, H2, A2, B2, C2, M2);
    }
    for (int i = 2; i < K; i++) {
        STEP(balloons, i, X, X1, X2, A3, B3, C3, M3);
        STEP(balloons, i, Y, Y1, Y2, A4, B4, C4, M4);
    }
}

auto solve() {
    read();
    sort(begin(towers), end(towers));
    sort(begin(balloons), end(balloons));
    vector<int> left(N, 0);  // sliding left from tower[i].P
    vector<int> right(N, 0); // sliding right from tower[i].P
    int P, H;

    // sliding left
    P = towers[N - 1].P;
    H = 0;
    for (int i = N - 1; i >= 0; i--) {
        int p = towers[i].P;
        int h = towers[i].H;
        if (h - p >= H - P) {
            H = h, P = p;
        }
        left[i] = H - (P - p);
    }

    // sliding right
    P = towers[0].P;
    H = 0;
    for (int i = 0; i < N; i++) {
        int p = towers[i].P;
        int h = towers[i].H;
        if (h + p >= H + P) {
            H = h, P = p;
        }
        right[i] = H - (p - P);
    }

    int k = 0; // tower[k - 1] < balloon <= tower[k].  0 <= k <= N.
    int captured = 0;
    for (int i = 0; i < K; i++) {
        int X = balloons[i].X, Y = balloons[i].Y;
        while (k < N && X > towers[k].P) {
            ++k;
        }
        // captured sliding left / captured sliding right
        bool cap_left = k < N && Y <= left[k] - (towers[k].P - X);
        bool cap_right = k > 0 && Y <= right[k - 1] - (X - towers[k - 1].P);
        captured += cap_left || cap_right;
    }
    return captured;
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
