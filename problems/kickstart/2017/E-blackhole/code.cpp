#include <bits/stdc++.h>

using namespace std;

// *****

// following analysis

struct Point3D {
    long X, Y, Z;
};

struct Point2D {
    double X, Y;
};

double dist(Point3D p1, Point3D p2) {
    long dx = p1.X - p2.X;
    long dy = p1.Y - p2.Y;
    long dz = p1.Z - p2.Z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

double dist(Point2D p1, Point2D p2) {
    double dx = p1.X - p2.X;
    double dy = p1.Y - p2.Y;
    return sqrt(dx * dx + dy * dy);
}

int intersect(Point2D p1, double r1, Point2D p2, double r2, Point2D &i1, Point2D &i2) {
    double d = dist(p1, p2);
    assert(d > 0.0);
    double dif = abs(r1 - r2);
    if (d > r1 + r2 || dif > d) {
        return 0;
    }
    double delta = r1 * r1 - r2 * r2;
    double d1 = (d + delta / d) / 2.0;
    double d2 = (d - delta / d) / 2.0;
    // intersection of [p1p2] with circle bisector line
    double X = (p1.X * d2 + p2.X * d1) / d;
    double Y = (p1.Y * d2 + p2.Y * d1) / d;
    // the circles intersect on [p1p2]
    if (abs(d - (r1 + r2)) <= DBL_EPSILON || abs(d - dif) <= DBL_EPSILON) {
        i1 = {X, Y};
        return 1;
    }
    double h = sqrt((r1 - d1) * (r1 + d1));
    double Xh = h * (p2.Y - p1.Y) / d;
    double Yh = h * (p1.X - p2.X) / d;
    i1 = {X + Xh, Y + Yh};
    i2 = {X - Xh, Y - Yh};
    return 2;
}

// whether an intersection of O(p1,r1) and O(p2,r2) is inside O(p3,r3)
bool cross_inside(Point2D p1, double r1, Point2D p2, double r2, Point2D p3, double r3) {
    Point2D i1, i2;
    int count = intersect(p1, r1, p2, r2, i1, i2);
    // intersection i1 is inside O(p3,r3)
    if (count >= 1 && dist(i1, p3) <= r3) {
        return true;
    }
    // intersection i2 is inside O(p3,r3)
    if (count >= 2 && dist(i2, p3) <= r3) {
        return true;
    }
    return false;
}

// whether p1 is inside O(p2,r2) and O(p3,r3)
bool circle_inside(Point2D p1, Point2D p2, double r2, Point2D p3, double r3) {
    // p1 inside O(p2,r2) and O(p3,r3)
    return dist(p1, p2) <= r2 && dist(p1, p3) <= r3;
}

// whether there exists a point inside all circles
bool exists_common_point(Point2D p1, double r1, Point2D p2, double r2, Point2D p3,
                         double r3) {
    return cross_inside(p1, r1, p2, r2, p3, r3) || //
           cross_inside(p2, r2, p3, r3, p1, r1) || //
           cross_inside(p3, r3, p1, r1, p2, r2) || //
           circle_inside(p1, p2, r2, p3, r3) ||    //
           circle_inside(p2, p3, r3, p1, r1) ||    //
           circle_inside(p3, p1, r1, p2, r2);
}

auto solve() {
    Point3D P1, P2, P3;
    cin >> P1.X >> P1.Y >> P1.Z;
    cin >> P2.X >> P2.Y >> P2.Z;
    cin >> P3.X >> P3.Y >> P3.Z;

    double d12 = dist(P1, P2);
    double d13 = dist(P1, P3);
    double d23 = dist(P2, P3);

    // now we want d12 >= d13 >= d23

    double dmax = std::max(d12, std::max(d13, d23));
    if (d23 >= dmax) {
        swap(P1, P3);
        swap(d23, d12);
    } else if (d13 >= dmax) {
        swap(P2, P3);
        swap(d13, d12);
    }
    if (d23 > d13) {
        swap(P1, P2);
        swap(d13, d23);
    }

    double m = d12 / 2.0; // never zero by input specification
    double x = (d13 * d13 - d23 * d23) / (4.0 * m);
    double y = sqrt(d13 * d13 - (x + m) * (x + m));

    const Point2D U = {-m, 0};
    const Point2D V = {+m, 0};
    const Point2D W = {x, y};

    double r1, r2;
    const int num_iterations = 40 + max(0.0, ceil(log2(dmax)));

    double lo = 0.0, hi = dmax;
    int iterations = num_iterations;

    // format 1: one circle around U, one around V, one around W
    // let C = C(W) be the center of the circle around W with radius r
    // we want to place C in the plane such that CU <= 3r and CV <= 3r
    // CU <= 3r ==> a circle around U with radius 3r contains C
    // CV <= 3r ==> a circle around V with radius 3r contains C
    // hence C lies inside all three circles. Any such point C ensures r is ok.
    do {
        double r = (lo + hi) / 2.0;
        if (exists_common_point(U, 3 * r, V, 3 * r, W, r)) {
            hi = r;
        } else {
            lo = r;
        }
    } while (--iterations);
    r1 = hi;

    lo = 0.0, hi = dmax;
    iterations = num_iterations;

    // format 2: one circle around U, one around V and W
    // let C = C(VW) be the center of the circle around V and W with radius r
    // we want to place C in the plane such that CU <= 5r
    // CU <= 5r ==> a circle around U with radius 5r contains C
    // hence C lies inside both circles. Any such point C ensures r is ok.
    do {
        double r = (lo + hi) / 2.0;
        if (exists_common_point(U, 5 * r, V, r, W, r)) {
            hi = r;
        } else {
            lo = r;
        }
    } while (--iterations);
    r2 = hi;

    return min(r1, r2);
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(9);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
