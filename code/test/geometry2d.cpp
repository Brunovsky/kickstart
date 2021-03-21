#include "../geometry2d.hpp"

#include "../random.hpp"
#include "test_utils.hpp"

// *****

using vecP = vector<P>;

double random_p() { return reald(0, 1)(mt); }
double random_real() { return reald(-10000, 10000)(mt); }

P random_point() {
    reald d(-1e15, 1e15);
    return P(d(mt), d(mt));
}
vector<double> random_reals(int N) {
    vector<double> rs(N);
    generate(begin(rs), end(rs), random_real);
    return rs;
}
vecP random_points(int N) {
    vecP ps(N);
    generate(begin(ps), end(ps), random_point);
    return ps;
}

void unit_test_collinear() {
    for (P a : random_points(100)) {
        for (P b : random_points(100)) {
            double k = random_p();
            P c = a * (1 - k) + b * k;
            assert(collinear(a, b, c));
            assert(collinear(b, a, c));
            assert(onsegment(a, c, b));
            P d = a * (1.1 - k) + b * k;
            assert(!collinear(a, b, d));
            assert(!onsegment(a, d, b));
        }
    }
    print_ok("unit test collinear");
}

void unit_test_circles() {
    P c = circumcenter(P(6, -4), P(8.4, -11.2), P(2, -8));
    double r = circumradius(P(6, -4), P(8.4, -11.2), P(2, -8));

    assert(c == P(6, -8));
    assert(abs(r - 4.0) <= eps);
    assert(intersect(c, 4, P(2, 8), P(10, -16)) == vecP({P(6, -4), P(8.4, -11.2)}));

    print_ok("unit test collinear");
}

int main() {
    unit_test_collinear();
    unit_test_circles();
    return 0;
}
