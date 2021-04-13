#ifndef FRAC_POINT2D_HPP
#define FRAC_POINT2D_HPP

#include "../hash.hpp"
#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"

// *****

/**
 * Class to represent points, lines, axis, circles etc in 3d using rational coordinates.
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 *                 https://cp-algorithms.com/geometry/basic-geometry.html
 */

template <typename F>
struct Point2D {
    F x, y, z;
    Point2D() = default;
    Point2D(const F& x, const F& y, const F& z) : x(x), y(y), z(z) {}
    Point2D(const array<F, 3>& a) : x(a[0]), y(a[1]), z(a[2]) {}

    using P = Point3D<F>;
    static inline const F inf = F(1, 0);
    static inline const P origin = P(0, 0, 0), pinf = P(inf, inf, inf);

    P operator-() const { return P(-x, -y, -z); }
    P operator+(const P& b) const { return P(x + b.x, y + b.y, z + b.z); }
    P operator-(const P& b) const { return P(x - b.x, y - b.y, z - b.z); }
    friend P operator*(const F& k, const P& a) { return P(k * a.x, k * a.y, k * a.z); }
    P operator*(const F& k) const { return P(k * x, k * y, k * z); }
    P operator/(const F& k) const { return P(x / k, y / k, z / k); }
    friend P& operator+=(P& a, const P& b) { return a = a + b; }
    friend P& operator-=(P& a, const P& b) { return a = a - b; }
    friend P& operator*=(P& a, const F& k) { return a = a * k; }
    friend P& operator/=(P& a, const F& k) { return a = a / k; }

    bool operator==(const P& b) const { return x == b.x && y == b.y && z == b.z; }
    bool operator!=(const P& b) const { return !(*this == b); }
    bool operator<(const P& b) const { return tie(x, y, z) < tie(b.x, b.y, b.z); }
    bool operator>(const P& b) const { return b < *this; }
    bool operator<=(const P& b) const { return !(b < *this); }
    bool operator>=(const P& b) const { return !(*this < b); }

    friend F dist2(const P& a) { return a.x * a.x + a.y * a.y + a.z * a.z; }
    friend F dist2(const P& a, const P& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    friend F dot(const P& a, const P& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    F xcross(const P& a, const P& b) const { return xcrossed(a - *this, b - *this); }
    F ycross(const P& a, const P& b) const { return ycrossed(a - *this, b - *this); }
    F zcross(const P& a, const P& b) const { return zcrossed(a - *this, b - *this); }
    P cross(const P& a, const P& b) const { return crossed(a - *this, b - *this); }
    friend F xcrossed(const P& a, const P& b) { return a.y * b.z - a.z * b.y; }
    friend F ycrossed(const P& a, const P& b) { return a.z * b.x - a.x * b.z; }
    friend F zcrossed(const P& a, const P& b) { return a.x * b.y - a.y * b.x; }
    friend P crossed(const P& a, const P& b) {
        return P(xcrossed(a, b), ycrossed(a, b), zcrossed(a, b));
    }

    friend bool collinear(const P& a, const P& b, const P& c) {
        return crossed(a, b, c) == 0;
    }
    friend bool onsegment(const P& a, const P& b, const P& c) {
        return collinear(a, b, c) && dot(a - b, c - b) <= 0;
    }

    friend F area(const P& a, const P& b, const P& c) { return a.cross(b, c) / 2; }

    explicit operator bool() const noexcept { return !x && !y && !z; }
    bool boxed(const P& min, const P& max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y && min.z <= z &&
               z <= max.z;
    }

    friend string to_string(const P& a) {
        return '(' + to_string(a.x) + ',' + to_string(a.y) + ',' + to_string(a.z) + ')';
    }
    friend ostream& operator<<(ostream& out, const P& a) { return out << to_string(a); }
};

#endif // FRAC_POINT3D_HPP
