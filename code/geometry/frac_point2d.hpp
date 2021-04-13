#ifndef FRAC_POINT2D_HPP
#define FRAC_POINT2D_HPP

#include "../hash.hpp"
#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"

// *****

/**
 * Class to represent points, lines, axis, circles etc in 2d
 * using integer or rational coordinates.
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 *                 https://cp-algorithms.com/geometry/basic-geometry.html
 */

template <typename F>
struct Point2D {
    F x, y;
    Point2D() = default;
    Point2D(const F& x, const F& y) : x(x), y(y) {}
    Point2D(const pair<F, F>& p) : x(p.first), y(p.second) {}
    Point2D(const array<F, 2>& a) : x(a[0]), y(a[1]) {}

    using P = Point2D<F>;
    static inline const F inf = F(1, 0);
    static inline const P origin = P(0, 0), pinf = P(inf, inf);

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

    bool operator==(const P& b) const { return x == b.x && y == b.y; }
    bool operator!=(const P& b) const { return !(*this == b); }
    bool operator<(const P& b) const { return tie(x, y) < tie(b.x, b.y); }
    bool operator>(const P& b) const { return b < *this; }
    bool operator<=(const P& b) const { return !(b < *this); }
    bool operator>=(const P& b) const { return !(*this < b); }

    friend F dist2(const P& a) { return a.x * a.x + a.y * a.y; }
    friend F dist2(const P& a, const P& b) { return a.x * b.x + a.y * b.y; }
    friend F dot(const P& a, const P& b) { return a.x * b.x + a.y * b.y; }
    friend P perp(const P& a) { return P(-a.y, a.x); } // 90 degrees clockwise
    F cross(const P& a, const P& b) const { return crossed(a - *this, b - *this); }
    friend F crossed(const P& a, const P& b) { return a.x * b.y - a.y * b.x; }

    friend bool collinear(const P& a, const P& b, const P& c) {
        return crossed(a, b, c) == 0;
    }
    friend bool onsegment(const P& a, const P& b, const P& c) {
        return collinear(a, b, c) && dot(a - b, c - b) <= 0;
    }
    friend P intersect(const P& u, const P& v, const P& a, const P& b) {
        auto d = crossed(v - u, b - a);
        if (d == 0)
            return pinf;
        auto p = a.cross(v, b), q = a.cross(b, u);
        return (u * p + v * q) / d;
    }

    friend P circumcenter(const P& A, const P& B, const P& C) {
        P ac = C - A, ab = B - A;
        return A + perp(ac * dist2(ab) - ab * dist2(ac)) / (2 * crossed(ac, ab));
    }
    friend P orthocenter(const P& A, const P& B, const P& C) {
        P b = perp(A - C), c = perp(A - B);
        return intersect(B, B + b, C, C + c);
    }
    friend F area(const P& a, const P& b, const P& c) { return a.cross(b, c) / 2; }
    friend F area(const vector<P>& ps) {
        F ans = 0;
        for (int i = 0, n = ps.size(); i < n; i++) {
            auto u = i ? ps[i - 1] : ps[n - 1], v = ps[i];
            ans += (u.x - v.x) * (u.y + v.y);
        }
        return ans;
    }

    // y = Bx + C
    friend pair<F, F> slope_line(const P& u, const P& v) {
        const F &a = u.x, b = u.y, c = v.x, d = v.y;
        assert(u != v);
        if (a != c)
            return {(b - d) / (a - c), (a * d - b * c) / (a - c)};
        else
            return {inf, a};
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

#endif // FRAC_POINT2D_HPP
