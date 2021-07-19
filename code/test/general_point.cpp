#include "test_utils.hpp"
#include "../numeric/frac.hpp"

#pragma GCC diagnostic ignored "-Wfloat-equal"
#include "../geometry/point2d.hpp"
#include "../geometry/point3d.hpp"

template struct Point2d<frac, double>;
template struct Point2d<long, double>;
template struct Point2d<long, long double>;
template struct Point2d<__int128_t, double>;
template struct Point2d<__int128_t, long double>;
template struct Point2d<double, double>;
template struct Point2d<long double, long double>;

template struct Point3d<frac, double>;
template struct Point3d<long, double>;
template struct Point3d<long, long double>;
template struct Point3d<__int128_t, double>;
template struct Point3d<__int128_t, long double>;
template struct Point3d<double, double>;
template struct Point3d<long double, long double>;

template struct Plane<frac, double>;
template struct Plane<long, double>;
template struct Plane<long, long double>;
template struct Plane<__int128_t, double>;
template struct Plane<__int128_t, long double>;
template struct Plane<double, double>;
template struct Plane<long double, long double>;

using F = frac;
using P = Point2d<frac>;
using P3 = Point3d<frac>;

frac area2(P3 a, P3 b, P3 c) { return norm2(a.crossed(b, c)) / 4; }

void unit_test_point2d() {
    P A(1, 4), B(7, 0), C(6, 8);
    auto a2 = dist2(B, C), b2 = dist2(A, C), c2 = dist2(A, B);
    auto ha2 = linedist2(A, B, C);
    auto hb2 = linedist2(B, C, A);
    auto hc2 = linedist2(C, A, B);

    print("lengths: a2={} b2={} c2={}\n", a2, b2, c2);
    print("area: {}\n", area(A, B, C));
    print("dists: ha2={} hb2={} hc2={}\n", ha2, hb2, hc2);

    assert(a2 == 65 && b2 == 41 && c2 == 52);
    assert(ha2 == F(1936, 65) && hb2 == F(1936, 41) && hc2 == F(484, 13));
    assert(area(A, B, C) == 22);
}

void unit_test_point3d_2d() {
    P3 A(1, 4, 0), B(7, 0, 0), C(6, 8, 0);
    auto a2 = dist2(B, C), b2 = dist2(A, C), c2 = dist2(A, B);
    auto ha2 = linedist2(A, B, C);
    auto hb2 = linedist2(B, C, A);
    auto hc2 = linedist2(C, A, B);

    print("lengths: a2={} b2={} c2={}\n", a2, b2, c2);
    print("area2: {}\n", area2(A, B, C));
    print("dists: ha2={} hb2={} hc2={}\n", ha2, hb2, hc2);

    assert(a2 == 65 && b2 == 41 && c2 == 52);
    assert(ha2 == F(1936, 65) && hb2 == F(1936, 41) && hc2 == F(484, 13));
}

void unit_test_point3d_3d() {
    P3 A(1, 4, 8), B(7, 0, 13), C(6, 8, 9);
    auto a2 = dist2(B, C), b2 = dist2(A, C), c2 = dist2(A, B);
    auto ha2 = linedist2(A, B, C);
    auto hb2 = linedist2(B, C, A);
    auto hc2 = linedist2(C, A, B);

    print("lengths: a2={} b2={} c2={}\n", a2, b2, c2);
    print("area2: {}\n", area2(A, B, C));
    print("dists: ha2={} hb2={} hc2={}\n", ha2, hb2, hc2);

    assert(a2 == 9 * 9 && b2 == 42 && c2 == 77);
    assert(ha2 == F(2873, 81) && hb2 == F(2873, 42) && hc2 == F(2873, 77));
}

void unit_test_rot() {
    vector<P> ps = {
        P(0, 3),  P(4, 7),  P(-8, 4),    P(3, 9),   P(-2, 0),  P(0, -7),
        P(11, 9), P(8, 0),  P(4, 0),     P(2, 8),   P(-3, -7), P(-14, 17),
        P(6, -9), P(2, -1), P(-12, -15), P(11, -6), P(-4, 10), P(-10, -5),
    };

    sort(begin(ps), end(ps), [](auto a, auto b) { return angle_less(a, b); });

    for (int i = 0; i < int(ps.size()); i++) {
        cout << ps[i] << '\n';
    }
}

int main() {
    RUN_BLOCK(unit_test_point2d());
    RUN_BLOCK(unit_test_point3d_2d());
    RUN_BLOCK(unit_test_point3d_3d());
    RUN_BLOCK(unit_test_rot());
    return 0;
}
