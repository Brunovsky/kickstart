#ifndef POINT3D_HPP
#define POINT3D_HPP

#include "../hash.hpp"
#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"

// *****

/**
 * Class to represent points, lines, axis, circles etc in 3d.
 * First version uses rational coordinates, has no precision uses but limited interface.
 * Second version uses doubles, has precision issues in places but full interface.
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 *                 https://cp-algorithms.com/geometry/basic-geometry.html
 */

template <typename F>
struct Point3d {
    F x, y, z;
    Point3d() : x(0), y(0), z(0) {}
    Point3d(const F& x, const F& y, const F& z) : x(x), y(y), z(z) {}
    Point3d(const array<F, 3>& a) : x(a[0]), y(a[1]), z(a[2]) {}

    using P = Point3d<F>;
    static inline const F inf = F(1, 0); // positive infinity
    static inline const P origin = P(0, 0, 0);
    static inline const P pinf = P(inf, inf, inf); // point at infinity

    bool operator==(const P& b) const { return x == b.x && y == b.y && z == b.z; }
    bool operator!=(const P& b) const { return !(*this == b); }
    bool operator<(const P& b) const { return tie(x, y, z) < tie(b.x, b.y, b.z); }
    bool operator>(const P& b) const { return b < *this; }
    bool operator<=(const P& b) const { return !(b < *this); }
    bool operator>=(const P& b) const { return !(*this < b); }
    explicit operator bool() const noexcept { return *this != origin; }

    F& operator[](int i) { return assert(0 <= i && i < 3), *(&x + i); }
    const F& operator[](int i) const { return assert(0 <= i && i < 3), *(&x + i); }
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

    friend double dist(const P& a) { return sqrt(dist2(a)); }
    friend double dist(const P& a, const P& b) { return sqrt(dist2(a, b)); }
    F norm2() const { return dist2(*this); }
    friend F dist2(const P& a) { return dot(a, a); }
    friend F dist2(const P& a, const P& b) { return dist2(a - b); }
    friend F dot(const P& a, const P& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    friend F dot2(const P& a, const P& b) { return dot(a, b) * dot(a, b); }
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

    // -- Lines

    // Are points a, b, c collinear in any order? (degenerate=yes)
    friend bool collinear(const P& a, const P& b, const P& c) {
        return a.cross(b, c) == origin;
    }
    // Are point a, b, c collinear in this order? (degenerate=yes)
    friend bool onsegment(const P& a, const P& b, const P& c) {
        return collinear(a, b, c) && dot(a - b, c - b) <= 0;
    }
    // k=0 => a, k=1 => b, 0<k<1 inside segment [ab]
    friend P interpolate(const P& a, const P& b, const F& k) {
        return (1 - k) * a + k * b;
    }
    // Are vectors u and v parallel? (either way)
    friend bool parallel(const P& u, const P& v) {
        return dot2(u, v) == u.norm2() * v.norm2();
    }
    // Squared distance of a to line uv
    friend F linedist2(const P& a, const P& u, const P& v) {
        return a.cross(u, v).norm2() / dist2(u, v);
    }
    // Compute cos^2(angle(u,v))
    friend F cos2(const P& u, const P& v) { return dot2(u, v) / (u.norm2() * v.norm2()); }
    // Compute sin^2(angle(u,v))
    friend F sin2(const P& u, const P& v) { return 1 - cos2(u, v); }

    // -- Planes

    // Are points a, b, c, d coplanar in any order?
    friend bool coplanar(const P& a, const P& b, const P& c, const P& d) {
        return parallel(a.cross(c, d), b.cross(c, d));
    }
    // Is point P above (1), in (0) or below (-1) the plane by C with normal N?
    friend int planeside(const P& p, const P& c, const P& n) {
        F s = dot(n, p - c);
        return (s >= 0) - (s <= 0);
    }
    // Squared distance of p to this plane
    friend F planedist(const P& p, const P& c, const P& n) {
        F s = dot(n, p - c);
        return s * s / n.norm2();
    }

    // -- Area

    // Squared area of triangle abc
    friend F area2(const P& a, const P& b, const P& c) {
        return a.cross(b, c).norm2() / 4;
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
};

template <typename F>
struct Plane {
    using P = Point3d<F>;
    P n; // normal; null if plane is degenerate (e.g. 3 collinear points)
    F d; // distance to origin; plane equation: dot(n,x) + d = 0

    Plane() = default;
    Plane(const P& n, const F& d) : n(n), d(d) {}
    Plane(const P& a, const P& b, const P& c) : n(a.cross(b, c)), d(-dot(n, a)) {}

    void normalize() {
        if (n.x)
            d /= abs(n.x), n.y /= abs(n.x), n.z /= abs(n.x), n.x /= abs(n.x);
        else if (n.y)
            d /= abs(n.y), n.z /= abs(n.y), n.x /= abs(n.y), n.y /= abs(n.y);
        else if (n.z)
            d /= abs(n.z), n.x /= abs(n.z), n.y /= abs(n.z), n.z /= abs(n.z);
    }

    bool is_degenerate() const { return !n; }

    // True if same plane and same orientation
    bool same(const Plane& b) const {
        if (d != 0)
            return b.d != 0 && (d > 0) == (b.d > 0) && d * b.n == b.d * n;
        else
            return b.d == 0 && dot(n, b.n) > 0 && parallel(n, b.n);
    }
    // True if the same plane, possibly different orientation
    bool operator==(const Plane& b) const {
        if (d != 0)
            return b.d != 0 && d * b.n == b.d * n;
        else
            return b.d == 0 && parallel(n, b.n);
    }
    // True if different plane
    bool operator!=(const Plane& b) const { return !(*this == b); }

    // Concurrent plane with this one but opposite orientation
    Plane operator-() const { return Plane(-n, -d); }

    // Is point P above (1), in (0) or below (-1) this plane?
    int planeside(const P& p) {
        F s = dot(p, n) + d;
        return (s >= 0) - (s <= 0);
    }
    // Squared distance of p to this plane
    F planedist2(const P& p) const {
        F s = dot(p, n) + d;
        return s * s / n.norm2();
    }
    // Signed squared distance of p to this plane
    F signed_planedist2(const P& p) const {
        F s = dot(p, n) + d;
        return s > 0 ? s * s / n.norm2() : s < 0 ? -s * s / n.norm2() : 0;
    }
};

struct Hasher3d {
    template <typename F>
    size_t operator()(const Point3d<F>& p) const noexcept {
        return Hasher{}(array<F, 3>{p[0], p[1], p[2]});
    }
};

struct PlaneHasher {
    template <typename F>
    size_t operator()(const Plane<F>& plane) const noexcept {
        return Hasher{}(make_pair(plane.n, plane.d));
    }
};

namespace std {

template <typename F>
struct hash<Point3d<F>> : Hasher3d {};
template <typename F>
struct hash<Plane<F>> : PlaneHasher {};

} // namespace std

#endif // POINT3D_HPP
