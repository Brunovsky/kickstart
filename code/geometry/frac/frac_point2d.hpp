#ifndef FRAC_POINT2D_HPP
#define FRAC_POINT2D_HPP

#include "../../hash.hpp"
#include "../../numeric/bfrac.hpp"
#include "../../numeric/frac.hpp"

// *****

/**
 * Class to represent points, lines, axis, circles etc in 2d using frac, bfrac or long.
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 *                 https://cp-algorithms.com/geometry/basic-geometry.html
 */
template <typename T> // frac, bfrac or long long
struct Point2d {
    T x, y;
    Point2d() : x(0), y(0) {}
    Point2d(const T& x, const T& y) : x(x), y(y) {}
    Point2d(const pair<T, T>& p) : x(p.first), y(p.second) {}
    Point2d(const array<T, 2>& a) : x(a[0]), y(a[1]) {}

    using P = Point2d<T>;
    static P zero() { return P(0, 0); }
    static P one() { return P(1, 1); }

    bool operator==(const P& b) const { return x == b.x && y == b.y; }
    bool operator!=(const P& b) const { return !(*this == b); }
    explicit operator bool() const noexcept { return *this != zero(); }

    T& operator[](int i) { return assert(0 <= i && i < 2), *(&x + i); }
    const T& operator[](int i) const { return assert(0 <= i && i < 2), *(&x + i); }
    P operator-() const { return P(-x, -y); }
    P operator+(const P& b) const { return P(x + b.x, y + b.y); }
    P operator-(const P& b) const { return P(x - b.x, y - b.y); }
    friend P operator*(const T& k, const P& a) { return P(k * a.x, k * a.y); }
    P operator*(const T& k) const { return P(k * x, k * y); }
    P operator/(const T& k) const { return P(x / k, y / k); }
    friend P& operator+=(P& a, const P& b) { return a = a + b; }
    friend P& operator-=(P& a, const P& b) { return a = a - b; }
    friend P& operator*=(P& a, const T& k) { return a = a * k; }
    friend P& operator/=(P& a, const T& k) { return a = a / k; }

    T norm2() const { return dist2(*this); }
    friend T dot(const P& a, const P& b) { return a.x * b.x + a.y * b.y; }
    friend T dot2(const P& a, const P& b) { return dot(a, b) * dot(a, b); }
    friend double dist(const P& u) { return std::sqrt(double(dist2(u))); }
    friend double dist(const P& a, const P& b) { return std::sqrt(double(dist2(a, b))); }
    friend T dist2(const P& u) { return dot(u, u); }
    friend T dist2(const P& a, const P& b) { return dist2(a - b); }
    T manhattan() const { return abs(x) + abs(y); }
    friend T manhattan(const P& a, const P& b) { return (a - b).manhattan(); }
    friend P abs(const P& u) { return P(abs(u.x), abs(u.y)); }

    T cross(const P& a, const P& b) const { return crossed(a - *this, b - *this); }
    friend T crossed(const P& u, const P& v) { return u.x * v.y - u.y * v.x; }
    friend P rperp(const P& u) { return P(-u.y, u.x); } // 90 degrees counterclockwise
    friend P lperp(const P& u) { return P(u.y, -u.x); } // 90 degrees clockwise

    // -- Lines

    // Are points a, b, c collinear in any order? (degenerate=yes)
    friend bool collinear(const P& a, const P& b, const P& c) {
        return a.cross(b, c) == 0;
    }
    // Are point a, b, c collinear in this order? (degenerate=yes)
    friend bool onsegment(const P& a, const P& b, const P& c) {
        return collinear(a, b, c) && dot(a - b, c - b) <= 0;
    }
    // Are vectors u and v parallel? (either way)
    friend bool parallel(const P& u, const P& v) { return crossed(u, v) == zero(); }

    // Is point P on left of (1), on (0) or right of (-1) the semiline u->v?
    friend int lineside(const P& p, const P& u, const P& v) {
        auto c = u.cross(v, p);
        return (c >= 0) - (c <= 0);
    }

    // k=0 => a, k=1 => b, 0<k<1 inside segment [ab]
    friend P interpolate(const P& a, const P& b, const T& k) {
        return (1 - k) * a + k * b;
    }
    // Distance of a to line uv
    friend double linedist(const P& a, const P& u, const P& v) {
        return abs(double(a.cross(u, v))) / dist(u, v);
    }
    // Squared distance of a to line uv
    friend T linedist2(const P& a, const P& u, const P& v) {
        auto A = area(a, u, v);
        return 4 * A * A / dist2(u, v);
    }

    // Compute intersection of lines uv and ab (should not be parallel/concurrent)
    friend P intersect(const P& u, const P& v, const P& a, const P& b) {
        auto d = crossed(v - u, b - a);
        assert(d != 0);
        auto p = a.cross(v, b), q = a.cross(b, u);
        return (u * p + v * q) / d;
    }

    // -- Area
    // Oriented area of triangle abc (positive=ccw)
    friend T area(const P& a, const P& b, const P& c) { return a.cross(b, c) / 2; }
    // Oriented area of polygon, e.g. convex hull (positive=ccw)
    friend T area(const vector<P>& ps) {
        T ans = 0;
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

#endif // FRAC_POINT2D_HPP
