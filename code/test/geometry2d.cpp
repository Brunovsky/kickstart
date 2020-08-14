#include "../geometry2d.hpp"

// *****

mt19937 mt(random_device{}());
using reald = uniform_real_distribution<double>;

double random_p() {
    reald d(0, 1);
    return d(mt);
}
double random_real() {
    reald d(-1e4, 1e4);
    return d(mt);
}
P random_point() {
    reald d(-1e15, 1e15);
    return P(d(mt), d(mt));
}
vector<double> random_reals(int N) {
    vector<double> rs(N);
    generate(begin(rs), end(rs), random_real);
    return rs;
}
vector<P> random_points(int N) {
    vector<P> ps(N);
    generate(begin(ps), end(ps), random_point);
    return ps;
}

void test_collinear() {
    for (P a : random_points(100)) {
        for (P b : random_points(100)) {
            long k = random_p();
            P c = a * (1 - k) + b * k;
            assert(collinear(a, b, c));
            assert(collinear(b, a, c));
            assert(onsegment(a, c, b));
            P d = a * (1.1 - k) + b * k;
            assert(!collinear(a, b, d));
            assert(!onsegment(a, d, b));
        }
    }
}

void test_circles() {
    assert(intersect(P(6, -8), 4, P(2, 8), P(10, -16)) ==
           vector<P>({P(6, -4), P(8.4, -11.2)}));
    P c = circumcenter(P(6, -4), P(8.4, -11.2), P(2, -8));
    double r = circumradius(P(6, -4), P(8.4, -11.2), P(2, -8));
    cout << c << ' ' << r << endl;
    assert(c == P(6, -8));
    assert(abs(r - 4.0) <= eps);
}

int main() {
    test_collinear();
    test_circles();
    return 0;
}
