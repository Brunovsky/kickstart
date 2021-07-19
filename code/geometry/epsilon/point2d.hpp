#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Class to represent points, lines, axis, circles etc in 2d using doubles.
 * Primary sources: kactl, cp-alg, ecnerwala
 */
struct Point2d {
    double x, y;
    // int id = -1;

    Point2d() : x(0), y(0) {}
    Point2d(double x, double y) : x(x), y(y) {}
    // Point2d(double x, double y, int id) : x(x), y(y), id(id) {}

    using P = Point2d;
    static constexpr double inf = numeric_limits<double>::infinity();
    static constexpr double deps = 20 * numeric_limits<double>::epsilon();

    friend bool same(P a, P b, double eps = deps) {
        return dist(a, b) <= max(manh(a), manh(b)) * eps;
    }
    bool operator==(P b) const { return same(*this, b); }
    bool operator!=(P b) const { return !same(*this, b); }
    explicit operator bool() const noexcept { return *this != P(); }
    bool boxed(P min, P max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y;
    }
    friend P clamp(P p, P min, P max) {
        return P(clamp(p.x, min.x, max.x), clamp(p.y, min.y, max.y));
    }

    double& operator[](int i) { return assert(0 <= i && i < 2), *(&x + i); }
    double operator[](int i) const { return assert(0 <= i && i < 2), *(&x + i); }
    P operator-() const { return P(-x, -y); }
    P operator+() const { return P(x, y); }
    friend P operator+(P u, P v) { return P(u.x + v.x, u.y + v.y); }
    friend P operator-(P u, P v) { return P(u.x - v.x, u.y - v.y); }
    friend P operator*(double k, P u) { return P(u.x * k, u.y * k); }
    friend P operator*(P u, double k) { return P(u.x * k, u.y * k); }
    friend P operator/(P u, double k) { return P(u.x / k, u.y / k); }
    friend P& operator+=(P& u, P v) { return u = u + v; }
    friend P& operator-=(P& u, P v) { return u = u - v; }
    friend P& operator*=(P& u, double k) { return u = u * k; }
    friend P& operator/=(P& u, double k) { return u = u / k; }

    friend auto conj(P u) { return P(u.x, -u.y); } // complex conjugate
    friend auto perp_ccw(P u) { return P(-u.y, u.x); }
    friend auto perp_cw(P u) { return P(u.y, -u.x); }

    friend auto norm2(P u) { return u.x * u.x + u.y * u.y; }
    friend auto norm(P u) { return std::sqrt(norm2(u)); }
    friend auto unit(P u) { return u / norm(u); }
    auto& normalize() { return *this = unit(*this); }

    friend double manh(P u) { return abs(u.x) + abs(u.y); }
    friend double manh(P a, P b) { return manh(a - b); }
    friend auto abs(P u) { return P(abs(u.x), abs(u.y)); }

    friend auto dot(P u, P v) { return u.x * v.x + u.y * v.y; }
    auto doted(P a, P b) const { return dot(a - *this, b - *this); }

    friend auto cross(P u, P v) { return u.x * v.y - u.y * v.x; }
    auto crossed(P a, P b) const { return cross(a - *this, b - *this); }

    friend auto dot_cross(P u, P v) { return P(dot(u, v), cross(u, v)); }
    auto dot_crossed(P a, P b) const { return dot_cross(a - *this, b - *this); }

    friend double dist2(P a, P b) { return norm2(a - b); }
    friend double dist(P a, P b) { return std::sqrt(dist2(a, b)); }

    friend P interpolate(P a, P b, double k) { return a + (b - a) * k; }

  public: // Lattice points
    friend array<long, 2> round_lattice_point(P p) {
        return {long(round(p.x)), long(round(p.y))};
    }
    friend array<long, 2> floor_lattice_point(P p) {
        return {long(floor(p.x)), long(floor(p.y))};
    }
    friend array<long, 2> ceil_lattice_point(P p) {
        return {long(ceil(p.x)), long(ceil(p.y))}; //
    }

  public: // Angles (many from ecnerwala/kactl)
    auto rotate(double rad) const {
        return P(x * cos(rad) - y * sin(rad), x * sin(rad) + y * cos(rad));
    }
    auto rotate(double rad, P pivot) const { return (*this - pivot).rotate(rad) + pivot; }

    friend int quadrant(P p) {
        return p.x >= 0 && p.y >= 0  ? 0
               : p.x <= 0 && p.y > 0 ? 1
               : p.x < 0 && p.y <= 0 ? 2
                                     : 3;
    }

    // operator<(u,v) for angles, ccw, prefers smaller distances
    friend bool angle_less(P u, P v) {
        return quadrant(u) == quadrant(v)
                   ? cross(u, v) >= 0 ? cross(u, v) <= 0 ? norm(u) < norm(v) : 1 : 0
                   : quadrant(u) < quadrant(v);
    }

    // operator<(u,v) for angles as seen from pivot, ccw
    friend bool angle_less(P pivot, P u, P v) { return angle_less(u - pivot, v - pivot); }

    friend double angle(P u) { return atan2(u.y, u.x); }
    friend double cos(P u) { return clamp(u.x / norm(u), -1.0, 1.0); }
    friend double sin(P u) { return clamp(u.y / norm(u), -1.0, 1.0); }
    friend double angle(P u, P v) { return acos(cos(u, v)); }
    friend double cos(P u, P v) {
        return clamp(dot(u, v) / (norm(u) * norm(v)), -1.0, 1.0);
    }
    friend double sin(P u, P v) {
        return clamp(cross(u, v) / (norm(u) * norm(v)), -1.0, 1.0);
    }

  public: // Lines
    friend bool parallel(P u, P v, double eps = deps) {
        return collinear(P(), u, v, eps);
    }
    friend bool samedir(P u, P v, double eps = deps) {
        return parallel(u, v, eps) && dot(u, v) >= 0;
    }
    friend bool collinear(P a, P b, P c, double eps = deps) {
        double ab = dist(a, b), ac = dist(a, c), bc = dist(b, c);
        return ab >= max(ac, bc)   ? linedist(c, a, b) <= ab * eps
               : ac >= max(ab, bc) ? linedist(b, a, c) <= ac * eps
                                   : linedist(a, b, c) <= bc * eps;
    }
    // Are point a, b, c collinear in this order? (degenerate=yes)
    friend bool onsegment(P a, P b, P c, double eps = deps) {
        return collinear(a, b, c, eps) && dot(a - b, c - b) <= 0; // <=0, not eps
    }

    // Distance of a to line uv
    friend double signed_linedist(P a, P u, P v) { return a.crossed(u, v) / dist(u, v); }
    friend double linedist(P a, P u, P v) { return abs(a.crossed(u, v)) / dist(u, v); }

    // {B,C} such that y = Bx + C (possibly B=inf)
    friend pair<double, double> slope_line(P u, P v, double eps = deps) {
        const auto a = u.x, b = u.y, c = v.x, d = v.y;
        if (abs(a - c) <= max(abs(a), abs(c)) * eps)
            return {inf, a};
        return {(b - d) / (a - c), cross(u, v) / (a - c)};
    }
    // Compute intersection of lines uv and ab (point at infinity if parallel/concurrent)
    friend optional<P> intersect(P u, P v, P a, P b, double eps = deps) {
        auto d = cross(v - u, b - a);
        if (abs(d) <= dist(u, v) * dist(a, b) * eps)
            return std::nullopt;
        auto p = a.crossed(v, b), q = a.crossed(b, u);
        return (u * p + v * q) / d;
    }

    // Is point P on left of (1), on (0) or right of (-1) the semiline u->v?
    friend int lineside(P p, P u, P v, double eps = deps) {
        auto c = u.crossed(v, p);
        auto uv = dist2(u, v);
        return (c >= uv * eps) - (c <= -uv * eps);
    }

  public: // Area
    friend double area(P a, P b, P c) { return a.crossed(b, c) / 2; }
    friend double area(const vector<P>& ps) {
        double ans = 0;
        for (int i = 0, n = ps.size(); i < n; i++) {
            const auto &u = i ? ps[i - 1] : ps[n - 1], v = ps[i];
            ans += (u.x - v.x) * (u.y + v.y);
        }
        return ans;
    }

  public: // Format
    friend string to_string(P p) {
        return '(' + to_string(p.x) + ',' + to_string(p.y) + ')';
    }
    friend ostream& operator<<(ostream& out, P p) { return out << to_string(p); }
    friend istream& operator>>(istream& in, P& p) { return in >> p.x >> p.y; }
};
