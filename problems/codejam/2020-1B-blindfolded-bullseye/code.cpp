#include <bits/stdc++.h>

using namespace std;

// *****

enum ans_t { MISS = 0, HIT = 1, CENTER = 100 };

ans_t guess(long X, long Y) {
    printf("%ld %ld\n", X, Y);
    string response;
    cin >> response;
    if (response == "CENTER") {
        return CENTER;
    } else if (response == "HIT") {
        return HIT;
    } else if (response == "MISS") {
        return MISS;
    } else if (response == "WRONG") {
        fprintf(stderr, "WRONG\n");
    }
    exit(EXIT_FAILURE);
}

#define GUESS(ans, X, Y)    \
    auto ans = guess(X, Y); \
    if (ans == CENTER)      \
        return;

constexpr long C = 5e8;
constexpr long Q[9][2] = {
    {-C, C},  {0, C},  {C, C},  //
    {-C, 0},  {0, 0},  {C, 0},  //
    {-C, -C}, {0, -C}, {C, -C}, //
};

void solve() {
    int q = -1;
    for (int i = 0; i < 9; i++) {
        GUESS(ans, Q[i][0], Q[i][1]);
        if (ans == HIT) {
            q = i;
            break;
        }
    }
    fprintf(stderr, "Quadrant: %d\n", q);
    assert(q != -1);

    long Xq = Q[q][0], Yq = Q[q][1];
    long Xmin, Xmax, Ymin, Ymax;

    Xmin = -1e9, Xmax = Xq;
    while (Xmin <= Xmax) {
        long X = (Xmin + Xmax) / 2;
        GUESS(ans, X, Yq);
        if (ans == HIT) {
            Xmax = X - 1;
        } else {
            Xmin = X + 1;
        }
    }
    long Xlower = Xmin;

    Xmin = Xq, Xmax = 1e9;
    while (Xmin <= Xmax) {
        long X = (Xmin + Xmax) / 2;
        GUESS(ans, X, Yq);
        if (ans == HIT) {
            Xmin = X + 1;
        } else {
            Xmax = X - 1;
        }
    }
    long Xhigher = Xmax;

    Ymin = -1e9, Ymax = Yq;
    while (Ymin <= Ymax) {
        long Y = (Ymin + Ymax) / 2;
        GUESS(ans, Xq, Y);
        if (ans == HIT) {
            Ymax = Y - 1;
        } else {
            Ymin = Y + 1;
        }
    }
    long Ylower = Ymin;

    Ymin = Yq, Ymax = 1e9;
    while (Ymin <= Ymax) {
        long Y = (Ymin + Ymax) / 2;
        GUESS(ans, Xq, Y);
        if (ans == HIT) {
            Ymin = Y + 1;
        } else {
            Ymax = Y - 1;
        }
    }
    long Yhigher = Ymax;

    long X = (Xlower + Xhigher) / 2;
    long Y = (Ylower + Yhigher) / 2;
    fprintf(stderr, "Center: %ld %ld\n", X, Y);
    GUESS(ans, X, Y);
    exit(EXIT_FAILURE);
}

// *****

int main() {
    unsigned T;
    long A, B; // not used
    cin >> T >> A >> B;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
