#include <bits/stdc++.h>

using namespace std;

// *****

struct Obstacle {
    int X, Y;
};

struct range_t {
    double aunder = 0.0, aover = INFINITY;
};

bool operator<(range_t lhs, range_t rhs) {
    return tie(lhs.aunder, lhs.aover) < tie(rhs.aunder, rhs.aover);
}

int N, P, H;
vector<Obstacle> O;
double CEILING;

inline double f(double a, double x) {
    return a * (x * (P - x));
}

inline double a_through(double x, double y) {
    return y / (x * (P - x));
}

inline double dist(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2, dy = y1 - y2;
    return dx * dx + dy * dy;
}

double dist_parabola(double x, double y, double a) {
    double l = P / 2.0, r = P;
    constexpr double w13 = 1.0 / 3.0, w23 = 2.0 / 3.0;
    do {
        double X1 = w23 * l + w13 * r;
        double X2 = w13 * l + w23 * r;
        double D1 = dist(x, y, X1, f(a, X1));
        double D2 = dist(x, y, X2, f(a, X2));
        (D1 < D2) ? r = X2 : l = X1;
    } while (r - l > 1e-12);
    return dist(x, y, l, f(a, l));
}

void a_around(int x, int y, double R, double &aunder, double &aover) {
    bool cannot_go_under = y < R;
    bool cannot_go_over = x < R || x + R > P;
    aunder = cannot_go_under ? -1.0 : 0.0;
    aover = CEILING + 1.0;

    double aO = a_through(x, y);
    double aunder_max = aO;
    double aover_min = aO;
    cannot_go_over = cannot_go_over || aover_min >= CEILING;

    if (!cannot_go_under) {
        do {
            double a = 0.5 * (aunder + aunder_max);
            double PQ = dist_parabola(x, y, a);
            (PQ <= R * R) ? aunder_max = a : aunder = a;
        } while (aunder_max - aunder > 1e-12);
    }

    if (!cannot_go_over) {
        do {
            double a = 0.5 * (aover + aover_min);
            double PQ = dist_parabola(x, y, a);
            (PQ <= R * R) ? aover_min = a : aover = a;
        } while (aover - aover_min > 1e-12);
    }
}

bool check(double R) {
    vector<range_t> ranges(N);
    CEILING = a_through(P / 2.0, H - R);
    for (int i = 0; i < N; i++) {
        double &aunder = ranges[i].aunder;
        double &aover = ranges[i].aover;
        a_around(O[i].X, O[i].Y, R, aunder, aover);
    }
    sort(begin(ranges), end(ranges));
    double top = 0.0;
    for (int i = 0; i < N; i++) {
        double aunder = ranges[i].aunder;
        double aover = ranges[i].aover;
        if (aunder >= top) {
            return true;
        }
        top = max(top, aover);
    }
    return top <= CEILING;
}

auto solve() {
    cin >> N >> P >> H >> ws;
    O.resize(N);
    for (int i = 0; i < N; i++) {
        cin >> O[i].X >> O[i].Y >> ws;
        O[i].X = max(O[i].X, P - O[i].X);
    }

    double l = 0.0, r = H;
    do {
        double R = 0.5 * (l + r);
        check(R) ? l = R : r = R;
    } while (r - l > 1e-11);

    return 0.5 * (l + r);
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(7);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
