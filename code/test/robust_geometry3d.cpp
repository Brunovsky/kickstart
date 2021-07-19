#include "test_utils.hpp"
#include "../geometry/epsilon/point3d.hpp"
#include "../hash.hpp"

using P = Point3d;

inline namespace detail {

using vecP = vector<P>;
using truth_t = unordered_map<array<int, 2>, int>;

inline double rndp() { return reald(0, 1)(mt); }
inline double rndreal(double R) { return reald(-R, R)(mt); }
P random_point(double R) { return P(rndreal(R), rndreal(R), rndreal(R)); }

vecP random_points(int N, double R) {
    vecP ps(N);
    generate(begin(ps), end(ps), bind(random_point, R));
    return ps;
}

void print_table(const truth_t& expected, const truth_t& actual, int mx, int my) {
    static const char* cs[2][2] = {{"", "\033[31;1m"}, {"", "\033[0m"}};
    print(" x\\y");
    for (int ey = -my; ey <= my; ey++) {
        print("{:4}", ey);
    }
    print("\n");
    for (int ex = -mx; ex <= mx; ex++) {
        print("{:4}", ex);
        for (int ey = -my; ey <= my; ey++) {
            bool should = expected.at({ex, ey}), is = actual.at({ex, ey});
            bool wrong = should != is;
            print("{}{:4}{}", cs[0][wrong], is ? 1 : 0, cs[1][wrong]);
        }
        print("\n");
    }
}

} // namespace detail

void stress_test_collinear() {
    const int max_exp = 20;
    vector<vector<stringable>> table;
    table.push_back({"N", "R", "in", "around", "out"});

    auto run = [&](int N, double R) {
        printcl("stress test collinear N,R={},{}", N, R);
        int in = 0, around = 0, out = 0;

        for (P a : random_points(N, R)) {
            for (P b : random_points(N, R)) {
                P c1 = interpolate(a, b, rndp());
                P c2 = interpolate(a, b, rndreal(30));
                P c3 = interpolate(a, b, reald(1.2, 10)(mt));
                P perp = unit(P(c1.y * c1.z, c1.x * c1.z, -2 * c1.x * c1.y));
                P d = c1 + perp * R * 30 * P::deps;

                bool inok, arok, outok;
                inok = collinear(a, b, c1) && collinear(b, c1, a) &&
                       collinear(c1, a, b) && onsegment(a, c1, b) &&
                       parallel(a - c1, a - b) && parallel(b - c1, b - a) &&
                       parallel(c1 - a, c1 - b);
                arok = collinear(a, b, c2) && collinear(b, c2, a) &&
                       collinear(c2, a, b) && parallel(a - c2, a - b) &&
                       parallel(b - c2, b - a) && parallel(c2 - a, c2 - b);
                arok &= collinear(a, b, c3) && collinear(b, c3, a) &&
                        collinear(c3, a, b) && parallel(a - c3, a - b) &&
                        parallel(b - c3, b - a) && parallel(c3 - a, c3 - b) &&
                        !onsegment(a, c3, b);
                outok = !collinear(a, b, d) && !collinear(a, d, b) &&
                        !onsegment(a, d, b) && !parallel(a - d, b - d);

                in += inok, around += arok, out += outok;
            }
        }

        auto perc = [N](int n) { return format("{:6.2f}%", 100.0 * n / (N * N)); };
        table.push_back({N, format("{:5.0e}", R), perc(in), perc(around), perc(out)});
    };

    for (int e = -max_exp; e <= max_exp; e++) {
        run(100, pow(10, e));
    }

    print_time_table(table, "Collinear test");
}

void stress_test_coplanar() {
    const int max_exp = 20;
    vector<vector<stringable>> table;
    table.push_back({"N", "R", "in", "around", "out", "side", "sideplane"});

    auto run = [&](int N, double R) {
        printcl("stress test coplanar N,R={},{}", N, R);
        int in = 0, around = 0, out = 0;
        int side = 0, sideplane = 0;

        for (P a : random_points(N, R)) {
            for (P b : random_points(N, R)) {
                for (P c : random_points(N, R)) {
                    P d1 = c + rndp() * (a - c) + rndp() * (b - c);
                    P d2 = c + rndreal(20) * (a - c) + rndreal(20) * (b - c);
                    P hi = a + unit(a.crossed(b, c)) * R * 30 * P::deps;
                    P lo = a - unit(a.crossed(b, c)) * R * 30 * P::deps;
                    Plane plane(a, b, c);

                    bool inok, arok, outok, sideok, spok;
                    inok = coplanar(a, b, c, d1) && coplanar(b, d1, a, c);
                    arok = coplanar(a, b, c, d2) && coplanar(b, d2, a, c);
                    outok = !coplanar(a, b, c, hi) && !coplanar(b, hi, a, d1) &&
                            !coplanar(a, b, c, lo) && !coplanar(b, lo, a, d1);
                    sideok = planeside(d1, a, a.crossed(b, c)) == 0 &&
                             planeside(d2, a, a.crossed(b, c)) == 0 &&
                             planeside(hi, a, a.crossed(b, c)) == 1 &&
                             planeside(lo, a, a.crossed(b, c)) == -1;
                    spok = plane.planeside(d1) == 0 && plane.planeside(d2) == 0 &&
                           plane.planeside(hi) == 1 && plane.planeside(lo) == -1;

                    in += inok, out += outok, around += arok;
                    side += sideok, sideplane += spok;
                }
            }
        }

        auto perc = [N](int n) { return format("{:6.2f}%", 100.0 * n / (N * N * N)); };
        table.push_back({N, format("{:5.0e}", R), perc(in), perc(around), perc(out),
                         perc(side), perc(sideplane)});
    };

    for (int e = -max_exp; e <= max_exp; e++) {
        run(33, pow(10, e));
    }

    print_time_table(table, "Coplanar test");
}

void table_test_collinear(int mx = 20, int my = 20) {
    int wrongs = 0;
    truth_t expected, actual;
    for (int ex = -mx; ex <= mx; ex++) {
        double x = pow(10, ex);
        for (int ey = -my; ey <= my; ey++) {
            double y = pow(10, ey);

            bool should = min(x, y) <= max(x, y) * P::deps;
            bool is = collinear(P(0, 0, 0), P(x, 0, 0), P(0, y, 0));

            wrongs += is != should;
            expected[{ex, ey}] = should;
            actual[{ex, ey}] = is;
        }
    }

    if (wrongs >= 3) {
        print_table(expected, actual, mx, my);
    } else {
        print("table_test_collinear wrongs: {}\n", wrongs);
    }
}

void table_test_same(int mx = 20, int my = 20) {
    int wrongs = 0;
    truth_t expected, actual;
    for (int ex = -mx; ex <= mx; ex++) {
        double x = pow(10, ex);
        for (int ey = -my; ey <= my; ey++) {
            double y = pow(10, ey);

            bool should = y <= sqrt(3) * x * P::deps;
            bool is = same(P(x, x, x), P(x, x, x + y));

            wrongs += is != should;
            expected[{ex, ey}] = should;
            actual[{ex, ey}] = is;
        }
    }

    if (wrongs >= 3) {
        print_table(expected, actual, mx, my);
    } else {
        print("table_test_same wrongs: {}\n", wrongs);
    }
}

void table_test_coplanar(int mx = 20, int my = 20) {
    int wrongs = 0;
    truth_t expected, actual;
    for (int ex = -mx; ex <= mx; ex++) {
        double x = pow(10, ex);
        for (int ey = -my; ey <= my; ey++) {
            double y = pow(10, ey);

            bool should = min(x, y) <= sqrt(3) * max(x, y) * P::deps;
            bool is = coplanar(P(0, x, x), P(x, 0, x), P(x, x, x),
                               P(x / 2, x / 2, x + y));

            wrongs += is != should;
            expected[{ex, ey}] = should;
            actual[{ex, ey}] = is;
        }
    }

    if (wrongs >= 3) {
        print_table(expected, actual, mx, my);
    } else {
        print("table_test_coplanar wrongs: {}\n", wrongs);
    }
}

void table_test_planeside(int mx = 20, int my = 20) {
    int wrongs = 0;
    truth_t expected, actual;
    for (int ex = -mx; ex <= mx; ex++) {
        double x = pow(10, ex);
        for (int ey = -my; ey <= my; ey++) {
            double y = pow(10, ey);
            Plane plane(P(0, x, x), P(x, 0, x), P(x, x, x));

            bool should = y <= sqrt(2) * x * P::deps; // not the same as coplanar!
            bool is = plane.planeside(P(x / 2, x / 2, x + y)) == 0;

            wrongs += is != should;
            expected[{ex, ey}] = should;
            actual[{ex, ey}] = is;
        }
    }

    if (wrongs >= 3) {
        print_table(expected, actual, mx, my);
    } else {
        print("table_test_planeside wrongs: {}\n", wrongs);
    }
}

void table_test_same_plane(int mx = 20, int my = 20) {
    int wrongs = 0;
    truth_t expected, actual;
    for (int ex = -mx; ex <= mx; ex++) {
        double x = pow(10, ex);
        for (int ey = -my; ey <= my; ey++) {
            double y = pow(10, ey);
            Plane a(P(0, x, x), P(x, 0, x), P(x, x, x));
            Plane b(P(0, x, x - y), P(x, 0, x - y), P(x, x, x + y));

            bool should = y <= sqrt(2) * x * P::deps;
            bool is = same_oriented(a, b);

            wrongs += is != should;
            expected[{ex, ey}] = should;
            actual[{ex, ey}] = is;
        }
    }

    if (wrongs >= 3) {
        print_table(expected, actual, mx, my);
    } else {
        print("table_test_same_plane wrongs: {}\n", wrongs);
    }
}

int main() {
    print("Epsilon Point3d: {}\n", P::deps);
    RUN_SHORT(table_test_collinear());
    RUN_SHORT(table_test_same());
    RUN_SHORT(table_test_coplanar());
    RUN_SHORT(table_test_planeside());
    RUN_SHORT(table_test_same_plane());
    RUN_SHORT(stress_test_collinear());
    RUN_SHORT(stress_test_coplanar());
    return 0;
}
