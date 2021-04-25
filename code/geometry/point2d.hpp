#ifndef POINT2D_HPP
#define POINT2D_HPP

#include "../hash.hpp"
#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"

// *****

/**
 * Class to represent points, lines, axis, circles etc in 2d.
 * First version uses rational coordinates, has no precision uses but limited interface.
 * Second version uses doubles, has precision issues in places but full interface.
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 *                 https://cp-algorithms.com/geometry/basic-geometry.html
 */

template <typename F>
struct Point2d {
    F x, y;
    Point2d() = default;
    Point2d(const F& x, const F& y) : x(x), y(y) {}
    Point2d(const pair<F, F>& p) : x(p.first), y(p.second) {}
    Point2d(const array<F, 2>& a) : x(a[0]), y(a[1]) {}

    using P = Point2d<F>;
    static inline const F inf = F(1, 0); // positive infinity
    static inline const P origin = P(0, 0);
    static inline const P pinf = P(inf, inf); // point at infinity

    bool operator==(const P& b) const { return x == b.x && y == b.y; }
    bool operator!=(const P& b) const { return !(*this == b); }
    bool operator<(const P& b) const { return tie(x, y) < tie(b.x, b.y); }
    bool operator>(const P& b) const { return b < *this; }
    bool operator<=(const P& b) const { return !(b < *this); }
    bool operator>=(const P& b) const { return !(*this < b); }

    F& operator[](int i) { return assert(0 <= i && i < 2), *(&x + i); }
    const F& operator[](int i) const { return assert(0 <= i && i < 2), *(&x + i); }
    P operator-() const { return P(-x, -y); }
    P operator+(const P& b) const { return P(x + b.x, y + b.y); }
    P operator-(const P& b) const { return P(x - b.x, y - b.y); }
    friend P operator*(const F& k, const P& a) { return P(k * a.x, k * a.y); }
    P operator*(const F& k) const { return P(k * x, k * y); }
    P operator/(const F& k) const { return P(x / k, y / k); }
    friend P& operator+=(P& a, const P& b) { return a = a + b; }
    friend P& operator-=(P& a, const P& b) { return a = a - b; }
    friend P& operator*=(P& a, const F& k) { return a = a * k; }
    friend P& operator/=(P& a, const F& k) { return a = a / k; }

    F norm2() const { return dist2(*this); }
    friend double dist(const P& a) { return sqrt(dist2(a)); }
    friend double dist(const P& a, const P& b) { return sqrt(dist2(a, b)); }
    friend F dist2(const P& a) { return a.x * a.x + a.y * a.y; }
    friend F dist2(const P& a, const P& b) {
        auto dx = a.x - b.x, dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    friend F dot(const P& a, const P& b) { return a.x * b.x + a.y * b.y; }
    friend P perp(const P& a) { return P(-a.y, a.x); }  // 90 degrees clockwise
    friend P rperp(const P& a) { return P(a.y, -a.x); } // 90 degrees counterclockwise
    F cross(const P& a, const P& b) const { return crossed(a - *this, b - *this); }
    friend F crossed(const P& a, const P& b) { return a.x * b.y - a.y * b.x; }

    // -- Lines
    // Are points a, b, c collinear in any order? (degenerate=yes)
    friend bool collinear(const P& a, const P& b, const P& c) {
        return a.cross(b, c) == 0;
    }
    // Are point a, b, c collinear in this order? (degenerate=yes)
    friend bool onsegment(const P& a, const P& b, const P& c) {
        return collinear(a, b, c) && dot(a - b, c - b) <= 0;
    }
    // Compute intersection of lines uv and ab (point at infinity if parallel/concurrent)
    friend P intersect(const P& u, const P& v, const P& a, const P& b) {
        auto d = crossed(v - u, b - a);
        if (d == 0)
            return pinf;
        auto p = a.cross(v, b), q = a.cross(b, u);
        return (u * p + v * q) / d;
    }
    // k=0 => a, k=1 => b, 0<k<1 inside segment [ab]
    friend P interpolate(const P& a, const P& b, const F& k) {
        return (1 - k) * a + k * b;
    }
    // squared distance of a to line uv
    friend F linedist2(const P& a, const P& u, const P& v) {
        auto A = area(a, u, v);
        return 4 * A * A / dist2(u, v);
    }
    // {B,C} such that y = Bx + C (possibly B=inf)
    friend pair<F, F> slope_line(const P& u, const P& v) {
        const auto a = u.x, b = u.y, c = v.x, d = v.y;
        assert(u != v);
        if (a != c)
            return {(b - d) / (a - c), (a * d - b * c) / (a - c)};
        else
            return {inf, a};
    }

    // -- Triangles
    friend P circumcenter(const P& A, const P& B, const P& C) {
        P ac = C - A, ab = B - A;
        return A + perp(ac * dist2(ab) - ab * dist2(ac)) / (2 * crossed(ac, ab));
    }
    friend P orthocenter(const P& A, const P& B, const P& C) {
        P b = perp(A - C), c = perp(A - B);
        return intersect(B, B + b, C, C + c);
    }

    // -- Area
    // Oriented area of triangle abc (positive=ccw)
    friend F area(const P& a, const P& b, const P& c) { return a.cross(b, c) / 2; }
    // Squared area of triangle abc
    friend F area2(const P& a, const P& b, const P& c) {
        return a.cross(b, c) * a.cross(b, c) / 4;
    }
    // Oriented area of polygon, e.g. convex hull (positive=ccw)
    friend F area(const vector<P>& ps) {
        F ans = 0;
        for (int i = 0, n = ps.size(); i < n; i++) {
            auto u = i ? ps[i - 1] : ps[n - 1], v = ps[i];
            ans += (u.x - v.x) * (u.y + v.y);
        }
        return ans;
    }

    explicit operator bool() const noexcept { return !x && !y; }
    bool boxed(const P& min, const P& max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y;
    }

    friend string to_string(const P& a) {
        return '(' + to_string(a.x) + ',' + to_string(a.y) + ')';
    }
    friend ostream& operator<<(ostream& out, const P& a) { return out << to_string(a); }
};

#endif // POINT2D_HPP
