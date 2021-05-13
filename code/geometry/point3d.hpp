#ifndef POINT3D_HPP
#define POINT3D_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Class to represent points, lines, axis, circles etc in 3d using doubles.
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 *                 https://cp-algorithms.com/geometry/basic-geometry.html
 */
struct Point3d {
    double x, y, z;
    constexpr Point3d() : x(0), y(0), z(0) {}
    constexpr Point3d(double x, double y, double z) : x(x), y(y), z(z) {}
    constexpr Point3d(const array<double, 3>& a) : x(a[0]), y(a[1]), z(a[2]) {}

    using P = Point3d;
    static constexpr double inf = numeric_limits<double>::infinity();
    static constexpr double deps = 30 * numeric_limits<double>::epsilon();

    static constexpr P zero() { return P(0, 0, 0); }
    static constexpr P one() { return P(1, 1, 1); }
    static constexpr P pinf() { return P(inf, inf, inf); }

    friend bool same(const P& a, const P& b, double eps = deps) {
        return dist(a, b) <= max(a.manhattan(), b.manhattan()) * eps;
    }
    bool operator==(const P& b) const { return same(*this, b); }
    bool operator!=(const P& b) const { return same(*this, b); }
    explicit operator bool() const noexcept { return *this != zero(); }

    double& operator[](int i) { return assert(0 <= i && i < 3), *(&x + i); }
    double operator[](int i) const { return assert(0 <= i && i < 3), *(&x + i); }
    P operator-() const { return P(-x, -y, -z); }
    P operator+(const P& u) const { return P(x + u.x, y + u.y, z + u.z); }
    P operator-(const P& u) const { return P(x - u.x, y - u.y, z - u.z); }
    friend P operator*(double k, const P& u) { return P(k * u.x, k * u.y, k * u.z); }
    P operator*(double k) const { return P(k * x, k * y, k * z); }
    P operator/(double k) const { return P(x / k, y / k, z / k); }
    friend P& operator+=(P& u, const P& v) { return u = u + v; }
    friend P& operator-=(P& u, const P& v) { return u = u - v; }
    friend P& operator*=(P& u, double k) { return u = u * k; }
    friend P& operator/=(P& u, double k) { return u = u / k; }

    P& normalize() { return *this /= norm(); }
    P unit() const { return *this / norm(); }
    double norm() const { return dist(*this); }
    double norm2() const { return dist2(*this); }
    friend auto dot(const P& u, const P& v) { return u.x * v.x + u.y * v.y + u.z * v.z; }
    friend auto dot2(const P& u, const P& v) { return dot(u, v) * dot(u, v); }
    friend double dist(const P& u) { return std::sqrt(dist2(u)); }
    friend double dist(const P& a, const P& b) { return std::sqrt(dist2(a, b)); }
    friend double dist2(const P& u) { return dot(u, u); }
    friend double dist2(const P& a, const P& b) { return dist2(a - b); }
    double manhattan() const { return abs(x) + abs(y) + abs(z); }
    friend double manhattan(const P& a, const P& b) { return (a - b).manhattan(); }
    friend P abs(const P& u) { return P(abs(u.x), abs(u.y), abs(u.z)); }

    P cross(const P& a, const P& b) const { return crossed(a - *this, b - *this); }
    friend P crossed(const P& u, const P& v) {
        return P(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
    }
    P normal(const P& a, const P& b) const { return cross(a, b).unit(); }
    friend P normal(const P& u, const P& v) { return crossed(u, v).unit(); }

    // -- Lattice points

    array<long, 3> round_lattice_point() const {
        return {long(round(x)), long(round(y)), long(round(z))};
    }
    array<long, 3> floor_lattice_point() const {
        return {long(floor(x)), long(floor(y)), long(floor(z))};
    }
    array<long, 3> ceil_lattice_point() const {
        return {long(ceil(x)), long(ceil(y)), long(ceil(z))};
    }

    // -- Angles

    // Azimuthal angle (longitude) to x-axis in interval [-pi, pi], ignoring z
    double phi() const { return atan2(y, x); }
    // Zenith angle (latitude) to the z-axis in interval [0, pi]
    double theta() const { return atan2(std::sqrt(x * x + y * y), z); }
    // Azimuthal angle (longitude) to x-axis in interval [-pi, pi], ignoring z
    friend double phi(const P& p) { return p.phi(); }
    // Zenith angle (latitude) to the z-axis in interval [0, pi]
    friend double theta(const P& p) { return p.theta(); }

    P rotated(double rad, P axis) const {
        double s = sin(rad), c = cos(rad);
        axis.normalize();
        return axis * dot(*this, axis) * (1 - c) + (*this) * c + crossed(*this, axis) * s;
    }
    P& rotate(double rad, P axis) { return *this = rotated(rad, axis); }

    friend double cos(const P& u, const P& v) {
        return clamp(dot(u, v) / std::sqrt(u.norm2() * v.norm2()), -1.0, 1.0);
    }
    friend double sin(const P& u, const P& v) {
        return clamp(crossed(u, v).norm() / std::sqrt(u.norm2() * v.norm2()), -1.0, 1.0);
    }

    // -- Lines

    friend bool collinear(const P& a, const P& b, const P& c, double eps = deps) {
        double ab = dist(a, b), ac = dist(a, c), bc = dist(b, c);
        return ab >= max(ac, bc)   ? linedist(c, a, b) <= ab * eps
               : ac >= max(ab, bc) ? linedist(b, a, c) <= ac * eps
                                   : linedist(a, b, c) <= bc * eps;
    }
    // Are points a, b, c collinear in this order? (degenerate=yes)
    friend bool onsegment(const P& a, const P& b, const P& c, double eps = deps) {
        return collinear(a, b, c, eps) && dot(a - b, c - b) <= 0; // <=0, not eps
    }
    friend bool parallel(const P& u, const P& v, double eps = deps) {
        return collinear(zero(), u, v, eps);
    }

    friend P interpolate(const P& a, const P& b, double k) { return a + (b - a) * k; }

    // Distance of a to line uv
    friend double linedist(const P& a, const P& u, const P& v) {
        return a.cross(u, v).norm() / dist(u, v);
    }

    // -- Planes
    friend bool coplanar(const P& a, const P& b, const P& c, const P& d,
                         double eps = deps) {
        P n = (a.cross(b, c) + b.cross(c, d) + c.cross(d, a) + d.cross(a, b));
        return !planeside(a, b, n, eps) || !planeside(b, c, n, eps) ||
               !planeside(c, d, n, eps) || !planeside(d, a, n, eps);
    }
    // Is point P above (1), in (0) or below (-1) the plane by C with normal n?
    friend int planeside(const P& p, const P& c, const P& n, double eps = deps) {
        double s = dot(n, p - c), k = c.norm() * n.norm();
        return (s >= k * 4 * eps) - (s <= -k * 4 * eps);
    }

    // -- Area

    friend double area(const P& a, const P& b, const P& c) {
        return a.cross(b, c).norm() / 2;
    }
    friend double volume(const P& a, const P& b, const P& c, const P& d) {
        return dot(a - d, crossed(b - d, c - d)) / 6;
    }

    bool boxed(const P& min, const P& max) const {
        return min.x <= x && x <= max.x && //
               min.y <= y && y <= max.y && //
               min.z <= z && z <= max.z;
    }

    friend string to_string(const P& a) {
        return '(' + to_string(a.x) + ',' + to_string(a.y) + ',' + to_string(a.z) + ')';
    }
    friend ostream& operator<<(ostream& out, const P& a) { return out << to_string(a); }
    friend istream& operator>>(istream& in, P& a) { return in >> a.x >> a.y >> a.z; }
};

struct Plane {
    using P = Point3d;
    P n;          // normal; constructors set norm to 1
    double d = 0; // distance to origin; plane equation: dot(n,x) + d = 0

    Plane() = default;
    Plane(const P& N, double d) : n(N.unit()), d(d / N.norm()) {}
    Plane(const P& N, const P& c) : n(N.unit()), d(-dot(n, c)) {}
    Plane(const P& a, const P& b, const P& c) : n(a.cross(b, c).unit()), d(-dot(n, a)) {}

    bool is_degenerate(double eps = Point3d::deps) const { return n.norm() <= eps; }

    // True if same plane and same orientation
    friend bool same_oriented(const Plane& a, const Plane& b,
                              double eps = Point3d::deps) {
        return (a.d >= 0) == (b.d >= 0) &&
               abs(a.d - b.d) <= max(abs(a.d), abs(b.d)) * eps && same(a.n, b.n, eps);
    }
    // True if same plane and same orientation
    bool operator==(const Plane& b) const { return same_oriented(*this, b); }
    // True if different plane or different orientation
    bool operator!=(const Plane& b) const { return !(*this == b); }

    // Same plane as this one but opposite orientation
    Plane operator-() const { return Plane(-n, -d); }

    // Is point P above (1), in (0) or below (-1) this plane?
    int planeside(const P& p, double eps = Point3d::deps) const {
        double s = dot(p, n) + d;
        return (s >= p.norm() * eps) - (s <= -p.norm() * eps);
    }

    double planedist(const P& p) const { return abs(dot(p, n) + d) / n.norm(); }
    double signed_planedist(const P& p) const { return (dot(p, n) + d) / n.norm(); }
};

#endif // POINT3D_HPP
