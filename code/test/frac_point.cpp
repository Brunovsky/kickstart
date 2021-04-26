#include "../geometry/frac_point2d.hpp"
#include "../geometry/frac_point3d.hpp"
#include "test_utils.hpp"

// *****

template struct Point3d<frac>;
template struct Point3d<bfrac>;
template struct Plane<frac>;
template struct Plane<bfrac>;
using F = frac;
using Pf = Point2d<F>;
using Pb = Point2d<bfrac>;
using P3f = Point3d<F>;
using P3b = Point3d<bfrac>;

void unit_test_point2d() {
    Pf A(1, 4), B(7, 0), C(6, 8);
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
    P3f A(1, 4, 0), B(7, 0, 0), C(6, 8, 0);
    auto a2 = dist2(B, C), b2 = dist2(A, C), c2 = dist2(A, B);
    auto ha2 = linedist2(A, B, C);
    auto hb2 = linedist2(B, C, A);
    auto hc2 = linedist2(C, A, B);

    print("lengths: a2={} b2={} c2={}\n", a2, b2, c2);
    print("area2: {}\n", area2(A, B, C));
    print("dists: ha2={} hb2={} hc2={}\n", ha2, hb2, hc2);

    assert(a2 == 65 && b2 == 41 && c2 == 52);
    assert(ha2 == F(1936, 65) && hb2 == F(1936, 41) && hc2 == F(484, 13));
    assert(area2(A, B, C) == 22 * 22);
}

void unit_test_point3d_3d() {
    P3f A(1, 4, 8), B(7, 0, 13), C(6, 8, 9);
    auto a2 = dist2(B, C), b2 = dist2(A, C), c2 = dist2(A, B);
    auto ha2 = linedist2(A, B, C);
    auto hb2 = linedist2(B, C, A);
    auto hc2 = linedist2(C, A, B);

    print("lengths: a2={} b2={} c2={}\n", a2, b2, c2);
    print("area2: {}\n", area2(A, B, C));
    print("dists: ha2={} hb2={} hc2={}\n", ha2, hb2, hc2);

    assert(a2 == 9 * 9 && b2 == 42 && c2 == 77);
    assert(ha2 == F(2873, 81) && hb2 == F(2873, 42) && hc2 == F(2873, 77));
    assert(area2(A, B, C) == F(2873, 4));
}

int main() {
    RUN_SHORT(unit_test_point2d());
    RUN_SHORT(unit_test_point3d_2d());
    RUN_SHORT(unit_test_point3d_3d());
    return 0;
}
