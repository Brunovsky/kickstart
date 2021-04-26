#ifndef POINT2D_HPP
#define POINT2D_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Class to represent points, lines, axis, circles etc in 2d using doubles.
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 *                 https://cp-algorithms.com/geometry/basic-geometry.html
 */
struct Point2d {
    double x, y;
    constexpr Point2d() : x(0), y(0) {}
    constexpr Point2d(double x, double y) : x(x), y(y) {}
    constexpr Point2d(const pair<double, double>& p) : x(p.first), y(p.second) {}
    constexpr Point2d(const array<double, 2>& a) : x(a[0]), y(a[1]) {}

    using P = Point2d;
    static inline constexpr double inf = numeric_limits<double>::infinity();
    static inline double deps = 1e-10; // default epsilon for comparisons etc (positive)

    static constexpr P zero() { return P(0, 0); }
    static constexpr P one() { return P(1, 1); }
    static constexpr P pinf() { return P(inf, inf); }

    friend bool same(const P& a, const P& b, double eps = deps) {
        return dist(a, b) <= eps;
    }
    bool operator==(const P& b) const { return same(*this, b); }
    bool operator!=(const P& b) const { return same(*this, b); }
    explicit operator bool() const noexcept { return *this != zero(); }

    double& operator[](int i) { return assert(0 <= i && i < 2), *(&x + i); }
    double operator[](int i) const { return assert(0 <= i && i < 2), *(&x + i); }
    P operator-() const { return P(-x, -y); }
    P operator+(const P& u) const { return P(x + u.x, y + u.y); }
    P operator-(const P& u) const { return P(x - u.x, y - u.y); }
    friend P operator*(double k, const P& u) { return P(k * u.x, k * u.y); }
    P operator*(double k) const { return P(k * x, k * y); }
    P operator/(double k) const { return P(x / k, y / k); }
    friend P& operator+=(P& u, const P& v) { return u = u + v; }
    friend P& operator-=(P& u, const P& v) { return u = u - v; }
    friend P& operator*=(P& u, double k) { return u = u * k; }
    friend P& operator/=(P& u, double k) { return u = u / k; }

    P& normalize() { return *this /= norm(); }
    P unit() const { return *this / norm(); }
    double norm() const { return dist(*this); }
    double norm2() const { return dist2(*this); }
    friend double dot(const P& u, const P& v) { return u.x * v.x + u.y * v.y; }
    friend double dot2(const P& u, const P& v) { return dot(u, v) * dot(u, v); }
    friend double dist(const P& u) { return std::sqrt(dist2(u)); }
    friend double dist(const P& a, const P& b) { return std::sqrt(dist2(a, b)); }
    friend double dist2(const P& u) { return dot(u, u); }
    friend double dist2(const P& a, const P& b) { return dist2(a - b); }

    double cross(const P& a, const P& b) const { return crossed(a - *this, b - *this); }
    friend double crossed(const P& u, const P& v) { return u.x * v.y - u.y * v.x; }
    friend P rperp(const P& u) { return P(-u.y, u.x); } // 90 degrees counterclockwise
    friend P lperp(const P& u) { return P(u.y, -u.x); } // 90 degrees clockwise

    // -- Lattice points

    // Round to nearest lattice point (e.g. for hashing)
    array<long, 2> round_lattice_point() const {
        return {long(round(x)), long(round(y))};
    }
    // Round to nearest lattice point, floor each coordinate (e.g. for hashing)
    array<long, 2> floor_lattice_point() const {
        return {long(floor(x)), long(floor(y))};
    }
    // Round to nearest lattice point, ceil each coordinate (e.g. for hashing)
    array<long, 2> ceil_lattice_point() const { return {long(ceil(x)), long(ceil(y))}; }

    // -- Angles

    // Angle with the x-axis in [-pi, pi], right-hand
    double angle() const { return atan2(y, x); }

    // Angle with the x-axis in [-pi, pi], right-hand
    double angle(const P& p) { return p.angle(); }

    P rotated(double rad) const {
        return P(x * cos(rad) - y * sin(rad), x * sin(rad) + y * cos(rad));
    }
    P rotated(double rad, const P& pivot) const {
        return (*this - pivot).rotated(rad) + pivot;
    }
    P& rotate(double rad) { return *this = rotated(rad); }
    P& rotate(double rad, const P& pivot) { return *this = rotated(rad, pivot); }

    friend double cos(const P& p) { return clamp(p.x / p.norm(), -1.0, 1.0); }
    friend double sin(const P& p) { return clamp(p.y / p.norm(), -1.0, 1.0); }
    friend double cos(const P& u, const P& v) {
        return clamp(dot(u, v) / (u.norm() * v.norm()), -1.0, 1.0);
    }
    friend double sin(const P& u, const P& v) {
        return clamp(crossed(u, v) / (u.norm() * v.norm()), -1.0, 1.0);
    }

    // -- Lines

    // Are points a, b, c collinear in any order? (degenerate=yes)
    friend bool collinear(const P& a, const P& b, const P& c, double eps = deps) {
        return a.cross(b, c) <= eps * eps;
    }
    // Are point a, b, c collinear in this order? (degenerate=yes)
    friend bool onsegment(const P& a, const P& b, const P& c, double eps = deps) {
        return collinear(a, b, c, eps) && dot(a - b, c - b) <= 0; // <=0, not eps
    }
    // Are vectors u and v parallel? (either way)
    friend bool parallel(const P& u, const P& v, double eps = deps) {
        return crossed(u, v) <= eps * eps;
    }

    // k<0 => before a, k=0 => a, k=1 => b, k>1 => after b, 0<k<1 => in segment [ab]
    friend P interpolate(const P& a, const P& b, double k) { return (1 - k) * a + k * b; }
    // Distance of a to line uv
    friend double linedist(const P& a, const P& u, const P& v) {
        return a.cross(u, v) / dist(u, v);
    }
    // {B,C} such that y = Bx + C (possibly B=inf)
    friend pair<double, double> slope_line(const P& u, const P& v, double eps = deps) {
        const auto a = u.x, b = u.y, c = v.x, d = v.y;
        assert(!same(u, v, eps));
        if (abs(a - c) > eps)
            return {(b - d) / (a - c), (a * d - b * c) / (a - c)};
        else
            return {inf, a};
    }

    // Compute intersection of lines uv and ab (point at infinity if parallel/concurrent)
    friend P intersect(const P& u, const P& v, const P& a, const P& b,
                       double eps = deps) {
        auto d = crossed(v - u, b - a);
        if (d <= eps)
            return pinf();
        auto p = a.cross(v, b), q = a.cross(b, u);
        return (u * p + v * q) / d;
    }

    // -- Area
    // Oriented area of triangle abc (positive=ccw)
    friend double area(const P& a, const P& b, const P& c) { return a.cross(b, c) / 2; }
    // Oriented area of polygon, e.g. convex hull (positive=ccw)
    friend double area(const vector<P>& ps) {
        double ans = 0;
        for (int i = 0, n = ps.size(); i < n; i++) {
            auto u = i ? ps[i - 1] : ps[n - 1], v = ps[i];
            ans += (u.x - v.x) * (u.y + v.y);
        }
        return ans;
    }

    bool boxed(const P& min, const P& max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y;
    }

    friend string to_string(const P& a) {
        return '(' + to_string(a.x) + ',' + to_string(a.y) + ')';
    }
    friend ostream& operator<<(ostream& out, const P& a) { return out << to_string(a); }
};

#endif // POINT2D_HPP
