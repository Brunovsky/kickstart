#include "../geometry/point2d.hpp"
#include "../hash.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

// *****

#define Z(x) +format(" " #x "={:6.2f}%", perc(x))
using P = Point2d;
using vecP = vector<P>;
using truth_t = unordered_map<array<int, 2>, bool>;

inline double rndp() { return reald(0, 1)(mt); }
inline double rndreal(double R) { return reald(-R, R)(mt); }
P random_point(double R) { return P(rndreal(R), rndreal(R)); }

vecP random_points(int N, double R) {
    vecP ps(N);
    generate(begin(ps), end(ps), bind(random_point, R));
    return ps;
}

void stress_test_collinear_run(int N, double R) {
    int in = 0, around = 0, out = 0;
    for (P a : random_points(N, R)) {
        for (P b : random_points(N, R)) {
            P c1 = interpolate(a, b, rndp());
            P c2 = interpolate(a, b, rndreal(30));
            P c3 = interpolate(a, b, reald(1.2, 10)(mt));
            P d = c1 + (a - b).rperp().unit() * R * 10 * P::deps;

            bool inok, arok, outok;
            inok = collinear(a, b, c1) && collinear(b, a, c1) && onsegment(a, c1, b) &&
                   parallel(a - c1, a - b) && parallel(b - c1, b - a) &&
                   parallel(c1 - a, c1 - b);
            arok = collinear(a, b, c2) && collinear(b, a, c2) &&
                   parallel(a - c2, a - b) && parallel(b - c2, b - a) &&
                   parallel(c2 - a, c2 - b);
            arok &= collinear(a, b, c3) && collinear(b, a, c3) &&
                    parallel(a - c3, a - b) && parallel(b - c3, b - a) &&
                    parallel(c3 - a, c3 - b) && !onsegment(a, c3, b);
            outok = !collinear(a, b, d) && !collinear(a, d, b) && !onsegment(a, d, b) &&
                    !parallel(a - d, b - d);

            in += inok, around += arok, out += outok;
        }
    }
    auto perc = [N](int n) { return 100.0 * n / (N * N); };
    print("collinear R={:5.0e}:", R);
    print("" Z(in) Z(around) Z(out) + "\n");
}

void stress_test_collinear(int me = 20) {
    for (int e = -me, t = 0, T = 2 * me + 1; e <= me; e++, t++) {
        print_progress(t, T, "stress test coplanar");
        double R = pow(10, e);
        stress_test_collinear_run(100, R);
    }
}

void print_tables(const truth_t& expected, const truth_t& actual, int mx, int my) {
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

void table_test_collinear(int mx = 20, int my = 20) {
    int wrongs = 0;
    truth_t expected, actual;
    for (int ex = -mx; ex <= mx; ex++) {
        double x = pow(10, ex);
        for (int ey = -my; ey <= my; ey++) {
            double y = pow(10, ey);

            bool should = min(x, y) <= max(x, y) * P::deps;
            bool is = collinear(P(0, 0), P(x, 0), P(0, y));

            expected[{ex, ey}] = should;
            actual[{ex, ey}] = is;
            wrongs += is != should;
        }
    }

    if (wrongs >= 3) {
        print_tables(expected, actual, mx, my);
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

            bool should = y <= sqrt(2) * x * P::deps;
            bool is = same(P(x, x), P(x, x + y));

            wrongs += is != should;
            expected[{ex, ey}] = should;
            actual[{ex, ey}] = is;
        }
    }

    if (wrongs >= 3) {
        print_tables(expected, actual, mx, my);
    } else {
        print("table_test_same wrongs: {}\n", wrongs);
    }
}

int main() {
    print("Epsilon Point2d: {}\n", P::deps);
    mt.seed(73);
    RUN_SHORT(table_test_collinear());
    RUN_SHORT(table_test_same());
    RUN_SHORT(stress_test_collinear());
    return 0;
}
