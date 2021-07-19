#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Class to represent points, lines, axis, circles etc in 3d using doubles.
 * Primary sources: kactl, cp-alg, ecnerwala
 */
struct Point3d {
    double x, y, z;
    // int id = -1;

    Point3d() : x(0), y(0), z(0) {}
    Point3d(double x, double y, double z) : x(x), y(y), z(z) {}
    // Point3d(double x, double y, double z, int id) : x(x), y(y), z(z), id(id) {}

    using P = Point3d;
    static constexpr double inf = numeric_limits<double>::infinity();
    static constexpr double deps = 30 * numeric_limits<double>::epsilon();

    friend bool same(P a, P b, double eps = deps) {
        return dist(a, b) <= max(manh(a), manh(b)) * eps;
    }
    bool operator==(P b) const { return same(*this, b); }
    bool operator!=(P b) const { return !same(*this, b); }
    explicit operator bool() const noexcept { return *this != P(); }
    bool boxed(P min, P max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y && min.z <= z &&
               z <= max.z;
    }
    friend P clamp(P p, P min, P max) {
        return P(clamp(p.x, min.x, max.x), clamp(p.y, min.y, max.y),
                 clamp(p.z, min.z, max.z));
    }

    double& operator[](int i) { return assert(0 <= i && i < 3), *(&x + i); }
    double operator[](int i) const { return assert(0 <= i && i < 3), *(&x + i); }
    P operator-() const { return P(-x, -y, -z); }
    P operator+() const { return P(x, y, z); }
    friend P operator+(P u, P v) { return P(u.x + v.x, u.y + v.y, u.z + v.z); }
    friend P operator-(P u, P v) { return P(u.x - v.x, u.y - v.y, u.z - v.z); }
    friend P operator*(double k, P u) { return P(k * u.x, k * u.y, k * u.z); }
    friend P operator*(P u, double k) { return P(k * u.x, k * u.y, k * u.z); }
    friend P operator/(P u, double k) { return P(u.x / k, u.y / k, u.z / k); }
    friend P& operator+=(P& u, P v) { return u = u + v; }
    friend P& operator-=(P& u, P v) { return u = u - v; }
    friend P& operator*=(P& u, double k) { return u = u * k; }
    friend P& operator/=(P& u, double k) { return u = u / k; }

    friend auto norm2(P u) { return u.x * u.x + u.y * u.y + u.z * u.z; }
    friend auto norm(P u) { return std::sqrt(norm2(u)); }
    friend auto unit(P u) { return u / norm(u); }
    auto& normalize() { return *this = unit(*this); }

    friend double manh(P u) { return abs(u.x) + abs(u.y) + abs(u.z); }
    friend double manh(P a, P b) { return manh(a - b); }
    friend auto abs(P u) { return P(abs(u.x), abs(u.y), abs(u.z)); }

    friend auto dot(P u, P v) { return u.x * v.x + u.y * v.y + u.z * v.z; }
    auto doted(P a, P b) const { return dot(a - *this, b - *this); }

    friend auto cross(P u, P v) {
        return P(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
    }
    auto crossed(P a, P b) const { return cross(a - *this, b - *this); }

    friend double dist2(P a, P b) { return norm2(a - b); }
    friend double dist(P a, P b) { return std::sqrt(dist2(a, b)); }

    friend P normal(P u, P v) { return unit(cross(u, v)); }

    friend P interpolate(P a, P b, double k) { return a + (b - a) * k; }

  public: // Lattice points
    friend array<long, 3> round_lattice_point(P p) {
        return {long(round(p.x)), long(round(p.y)), long(round(p.z))};
    }
    friend array<long, 3> floor_lattice_point(P p) {
        return {long(floor(p.x)), long(floor(p.y)), long(floor(p.z))};
    }
    friend array<long, 3> ceil_lattice_point(P p) {
        return {long(ceil(p.x)), long(ceil(p.y)), long(ceil(p.z))};
    }

  public: // Angles
    auto rotate(double rad, P axis) const {
        double s = sin(rad), c = cos(rad);
        axis.normalize();
        return axis * dot(*this, axis) * (1 - c) + (*this) * c + cross(*this, axis) * s;
    }

    friend double angle(P u, P v) { return acos(cos(u, v)); }
    friend double cos(P u, P v) {
        return clamp(dot(u, v) / (norm(u) * norm(v)), -1.0, 1.0);
    }
    friend double sin(P u, P v) {
        return clamp(norm(cross(u, v)) / (norm(u) * norm(v)), -1.0, 1.0);
    }
    // Azimuthal angle (longitude) to x-axis in interval [-pi, pi], ignoring z
    friend double phi(P p) { return atan2(p.y, p.x); }
    // Zenith angle (latitude) to the z-axis in interval [0, pi]
    friend double theta(P p) { return atan2(norm(p), p.z); }

  public: // -- Lines
    friend bool collinear(P a, P b, P c, double eps = deps) {
        double ab = dist(a, b), ac = dist(a, c), bc = dist(b, c);
        return ab >= max(ac, bc)   ? linedist(c, a, b) <= ab * eps
               : ac >= max(ab, bc) ? linedist(b, a, c) <= ac * eps
                                   : linedist(a, b, c) <= bc * eps;
    }
    // Are points a, b, c collinear in this order? (degenerate=yes)
    friend bool onsegment(P a, P b, P c, double eps = deps) {
        return collinear(a, b, c, eps) && dot(a - b, c - b) <= 0; // <=0, not eps
    }
    friend bool parallel(P u, P v, double eps = deps) {
        return collinear(P(), u, v, eps);
    }

    // Distance of a to line uv
    friend double linedist(P a, P u, P v) { return norm(a.crossed(u, v)) / dist(u, v); }

  public: // Planes
    friend bool coplanar(P a, P b, P c, P d, double eps = deps) {
        P n = (a.crossed(b, c) + b.crossed(c, d) + c.crossed(d, a) + d.crossed(a, b));
        return !planeside(a, b, n, eps) || !planeside(b, c, n, eps) ||
               !planeside(c, d, n, eps) || !planeside(d, a, n, eps);
    }
    // Is point P above (1), in (0) or below (-1) the plane by C with normal n?
    friend int planeside(P p, P c, P n, double eps = deps) {
        double s = dot(n, p - c), k = norm(c) * norm(n);
        return (s >= k * 4 * eps) - (s <= -k * 4 * eps);
    }

  public: // Area
    friend double area(P a, P b, P c) { return norm(a.crossed(b, c)) / 2; }
    friend double volume(P a, P b, P c, P d) {
        return dot(a - d, cross(b - d, c - d)) / 6;
    }

  public: // Format
    friend string to_string(P p) {
        return '(' + to_string(p.x) + ',' + to_string(p.y) + ',' + to_string(p.z) + ')';
    }
    friend ostream& operator<<(ostream& out, P p) { return out << to_string(p); }
    friend istream& operator>>(istream& in, P& p) { return in >> p.x >> p.y >> p.z; }
};

struct Plane {
    using P = Point3d;
    P n;          // normal; constructors set norm to 1
    double d = 0; // distance to origin; plane equation: dot(n,x) + d = 0

    Plane() = default;
    Plane(P N, double d) : n(unit(N)), d(d / norm(N)) {}
    Plane(P N, P c) : n(unit(N)), d(-dot(n, c)) {}
    Plane(P a, P b, P c) : n(unit(a.crossed(b, c))), d(-dot(n, a)) {}

    bool is_degenerate(double eps = Point3d::deps) const { return norm(n) <= eps; }

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
    int planeside(P p, double eps = Point3d::deps) const {
        double s = dot(p, n) + d;
        return (s >= norm(p) * eps) - (s <= -norm(p) * eps);
    }

    double planedist(P p) const { return abs(dot(p, n) + d) / norm(n); }
    double signed_planedist(P p) const { return (dot(p, n) + d) / norm(n); }
};
