#include <bits/stdc++.h>

using namespace std;

// *****

using double3 = array<double, 3>;
using mat3x3 = array<double3, 3>;
using point3d = double3;
using axis3d = double3;

mat3x3 make_mat3x3(const double3& a, const double3& b, const double3& c) {
    return mat3x3{a, b, c};
}

point3d operator*(const mat3x3& mat, const point3d& v) {
    point3d b;
    b[0] = v[0] * mat[0][0] + v[1] * mat[0][1] + v[2] * mat[0][2];
    b[1] = v[0] * mat[1][0] + v[1] * mat[1][1] + v[2] * mat[1][2];
    b[2] = v[0] * mat[2][0] + v[1] * mat[2][1] + v[2] * mat[2][2];
    return b;
}

mat3x3 operator*(const mat3x3& a, const mat3x3& b) {
    mat3x3 c;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j];
        }
    }
    return c;
}

double length(const point3d& p) {
    return sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
}

point3d norm(point3d p) {
    double l = length(p);
    if (l > 0) {
        p[0] /= l, p[1] /= l, p[2] /= l;
    }
    return p;
}

mat3x3 rotation_matrix(const axis3d& axis, double a) {
    assert(abs(length(axis) - 1.0) <= 1e-20);
    double ux = axis[0], uy = axis[1], uz = axis[2];
    // clang-format off
    return {
        double3{  cos(a) + ux * ux * (1-cos(a)),     ux * uy * (1-cos(a)) - uz * sin(a),   ux * uz * (1-cos(a)) + uy * sin(a)},
        double3{uy * ux * (1-cos(a)) + uz * sin(a),    cos(a) + uy * uy * (1-cos(a)),      uy * uz * (1-cos(a)) - ux * sin(a)},
        double3{uz * ux * (1-cos(a)) - uy * sin(a),  uz * uy * (1-cos(a)) + ux * sin(a),     cos(a) + uz * uz * (1-cos(a))},
    };
    // clang-format on
}

auto projected_area(const mat3x3& cube) {
    return 2.0 * (abs(cube[0][1]) + abs(cube[1][1]) + abs(cube[2][1]));
}

void debug_print(const mat3x3& cube) {
    printf("area: %lf\n", projected_area(cube));
    for (auto p : cube) {
        printf("(%-1.8lf,%-1.8lf,%-1.8lf) \t", p[0], p[1], p[2]);
        printf("dist origin: %lf\n", length(p));
    }
}

// *****

const mat3x3 unit_cube = make_mat3x3({0.5, 0, 0}, {0, 0.5, 0}, {0, 0, 0.5});
const axis3d rot_axis = {M_SQRT1_2, 0, -M_SQRT1_2};
double max_a;

void ternary_search_maximum() {
    double lo = 0, hi = M_PI_2;
    do {
        double a1 = (2 * lo + hi) / 3;
        double a2 = (lo + 2 * hi) / 3;
        auto cube1 = rotation_matrix(rot_axis, a1) * unit_cube;
        auto cube2 = rotation_matrix(rot_axis, a2) * unit_cube;
        auto area1 = projected_area(cube1);
        auto area2 = projected_area(cube2);
        if (area1 < area2) {
            lo = a1;
        } else {
            hi = a2;
        }
    } while (abs(hi - lo) > 1e-12);
    double a = (lo + hi) / 2;
    double area = projected_area(rotation_matrix(rot_axis, a) * unit_cube);
    double deg = 180.0 * a / M_PI;
    fprintf(stderr, "max a  = %lf  (%lf deg)\n", a, deg);
    fprintf(stderr, "area   = %lf\n", area);
    fprintf(stderr, "sin(a) = %lf\n", sin(a));
    fprintf(stderr, "cos(a) = %lf\n", cos(a));
    fprintf(stderr, "tan(a) = %lf\n", tan(a));
    max_a = a;
}

auto binary_search(double A) {
    double lo = 0, hi = max_a;
    mat3x3 cube;
    do {
        double a = (lo + hi) / 2.0;
        cube = rotation_matrix(rot_axis, a) * unit_cube;
        if (projected_area(cube) < A) {
            lo = a;
        } else {
            hi = a;
        }
    } while (abs(hi - lo) > 1e-12);
    return cube;
}

void solve() {
    double A;
    cin >> A;
    auto cube = binary_search(A);
    for (auto p : cube) {
        printf("%1.8lf %1.8lf %1.8lf\n", p[0], p[1], p[2]);
    }
}

// *****

int main() {
    ternary_search_maximum();
    cout << showpoint << fixed << setprecision(9);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":\n";
        solve();
    }
    return 0;
}
