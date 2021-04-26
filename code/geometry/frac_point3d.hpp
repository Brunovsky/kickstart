#ifndef FRAC_POINT3D_HPP
#define FRAC_POINT3D_HPP

#include "../hash.hpp"
#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"

// *****

/**
 * Class to represent points, lines, axis, circles etc in 3d using frac, bfrac or long.
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 *                 https://cp-algorithms.com/geometry/basic-geometry.html
 */

template <typename T> // frac, bfrac or long long
struct Point3d {
    T x, y, z;
    Point3d() : x(0), y(0), z(0) {}
    Point3d(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}
    Point3d(const array<T, 3>& a) : x(a[0]), y(a[1]), z(a[2]) {}

    using P = Point3d<T>;
    static P zero() { return P(0, 0, 0); }

    bool operator==(const P& b) const { return x == b.x && y == b.y && z == b.z; }
    bool operator!=(const P& b) const { return !(*this == b); }
    explicit operator bool() const noexcept { return *this != zero(); }

    T& operator[](int i) { return assert(0 <= i && i < 3), *(&x + i); }
    const T& operator[](int i) const { return assert(0 <= i && i < 3), *(&x + i); }
    P operator-() const { return P(-x, -y, -z); }
    P operator+(const P& b) const { return P(x + b.x, y + b.y, z + b.z); }
    P operator-(const P& b) const { return P(x - b.x, y - b.y, z - b.z); }
    friend P operator*(const T& k, const P& a) { return P(k * a.x, k * a.y, k * a.z); }
    P operator*(const T& k) const { return P(k * x, k * y, k * z); }
    P operator/(const T& k) const { return P(x / k, y / k, z / k); }
    friend P& operator+=(P& a, const P& b) { return a = a + b; }
    friend P& operator-=(P& a, const P& b) { return a = a - b; }
    friend P& operator*=(P& a, const T& k) { return a = a * k; }
    friend P& operator/=(P& a, const T& k) { return a = a / k; }

    double norm() const { return std::sqrt(double(norm2())); }
    T norm2() const { return dist2(*this); }
    friend T dot(const P& a, const P& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    friend T dot2(const P& a, const P& b) { return dot(a, b) * dot(a, b); }
    friend double dist(const P& u) { return std::sqrt(double(dist2(u))); }
    friend double dist(const P& a, const P& b) { return std::sqrt(double(dist2(a, b))); }
    friend T dist2(const P& a) { return dot(a, a); }
    friend T dist2(const P& a, const P& b) { return dist2(a - b); }
    T manhattan() const { return abs(x) + abs(y) + abs(z); }
    friend T manhattan(const P& a, const P& b) { return (a - b).manhattan(); }
    friend P abs(const P& u) { return P(abs(u.x), abs(u.y), abs(u.z)); }

    T xcross(const P& a, const P& b) const { return xcrossed(a - *this, b - *this); }
    T ycross(const P& a, const P& b) const { return ycrossed(a - *this, b - *this); }
    T zcross(const P& a, const P& b) const { return zcrossed(a - *this, b - *this); }
    P cross(const P& a, const P& b) const { return crossed(a - *this, b - *this); }
    friend T xcrossed(const P& a, const P& b) { return a.y * b.z - a.z * b.y; }
    friend T ycrossed(const P& a, const P& b) { return a.z * b.x - a.x * b.z; }
    friend T zcrossed(const P& a, const P& b) { return a.x * b.y - a.y * b.x; }
    friend P crossed(const P& a, const P& b) {
        return P(xcrossed(a, b), ycrossed(a, b), zcrossed(a, b));
    }

    // -- Lines

    // Are points a, b, c collinear in any order? (degenerate=yes)
    friend bool collinear(const P& a, const P& b, const P& c) {
        return a.cross(b, c) == zero();
    }
    // Are point a, b, c collinear in this order? (degenerate=yes)
    friend bool onsegment(const P& a, const P& b, const P& c) {
        return collinear(a, b, c) && dot(a - b, c - b) <= 0;
    }
    // Are vectors u and v parallel? (either way)
    friend bool parallel(const P& u, const P& v) { return crossed(u, v) == zero(); }

    // k=0 => a, k=1 => b, 0<k<1 inside segment [ab]
    friend P interpolate(const P& a, const P& b, const T& k) {
        return (1 - k) * a + k * b;
    }
    // Distance of a to line uv
    friend double linedist(const P& a, const P& u, const P& v) {
        return a.cross(u, v).norm() / dist(u, v);
    }
    // Squared distance of a to line uv
    friend T linedist2(const P& a, const P& u, const P& v) {
        return a.cross(u, v).norm2() / dist2(u, v);
    }
    friend double cos(const P& u, const P& v) {
        return clamp(dot(u, v) / (u.norm() * v.norm()), -1.0, 1.0);
    }
    friend double sin(const P& u, const P& v) {
        return clamp(crossed(u, v).norm() / (u.norm() * v.norm()), -1.0, 1.0);
    }
    friend T cos2(const P& u, const P& v) { return dot2(u, v) / (u.norm2() * v.norm2()); }
    friend T sin2(const P& u, const P& v) { return 1 - cos2(u, v); }

    // -- Planes

    // Are points a, b, c, d coplanar in any order?
    friend bool coplanar(const P& a, const P& b, const P& c, const P& d) {
        return parallel(a.cross(c, d), b.cross(c, d));
    }
    // Is point P above (1), in (0) or below (-1) the plane by C with normal N?
    friend int planeside(const P& p, const P& c, const P& n) {
        T s = dot(n, p - c);
        return (s >= 0) - (s <= 0);
    }

    // -- Area

    // Area of triangle abc
    friend double area(const P& a, const P& b, const P& c) {
        return a.cross(b, c).norm() / 2;
    }
    // Squared area of triangle abc
    friend T area2(const P& a, const P& b, const P& c) {
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

template <typename T>
struct Plane {
    using P = Point3d<T>;
    P n; // normal; null if plane is degenerate (e.g. 3 collinear points)
    T d; // distance to zero; plane equation: dot(n,x) + d = 0

    Plane() = default;
    Plane(const P& n, const T& d) : n(n), d(d) {}
    Plane(const P& a, const P& b, const P& c) : n(a.cross(b, c)), d(-dot(n, a)) {}

    void normalize() { // set the first non-zero coordinate of n to 1.
        if (n.x)
            d /= abs(n.x), n.y /= abs(n.x), n.z /= abs(n.x), n.x /= abs(n.x);
        else if (n.y)
            d /= abs(n.y), n.z /= abs(n.y), n.x /= abs(n.y), n.y /= abs(n.y);
        else if (n.z)
            d /= abs(n.z), n.x /= abs(n.z), n.y /= abs(n.z), n.z /= abs(n.z);
    }
    bool is_degenerate() const { return !n; }

    // True if same plane and same orientation
    friend bool same_oriented(const Plane& a, const Plane& b) {
        if (a.d != 0)
            return b.d != 0 && (a.d > 0) == (b.d > 0) && a.d * b.n == b.d * a.n;
        else
            return b.d == 0 && dot(a.n, b.n) > 0 && parallel(a.n, b.n);
    }
    // True if same plane and same orientation
    bool operator==(const Plane& b) const { return same_oriented(*this, b); }
    // True if different plane or different orientation
    bool operator!=(const Plane& b) const { return !(*this == b); }

    // Concurrent plane with this one but opposite orientation
    Plane operator-() const { return Plane(-n, -d); }

    // Is point P above (1), in (0) or below (-1) this plane?
    int planeside(const P& p) {
        T s = dot(p, n) + d;
        return (s >= 0) - (s <= 0);
    }

    double planedist(const P& p) const { return abs(double(dot(p, n) + d)) / n.norm(); }
    double signed_planedist(const P& p) const { return double(dot(p, n) + d) / n.norm(); }
    T planedist2(const P& p) const {
        T s = dot(p, n) + d;
        return s * s / n.norm2();
    }
    T signed_planedist2(const P& p) const {
        T s = dot(p, n) + d;
        return s > 0 ? s * s / n.norm2() : s < 0 ? -s * s / n.norm2() : 0;
    }
};

struct Hasher3d {
    template <typename T>
    size_t operator()(const Point3d<T>& p) const noexcept {
        return Hasher{}(array<T, 3>{p[0], p[1], p[2]});
    }
};

struct PlaneHasher {
    template <typename T>
    size_t operator()(const Plane<T>& plane) const noexcept {
        return Hasher{}(make_pair(plane.n, plane.d));
    }
};

namespace std {

template <typename T>
struct hash<Point3d<T>> : Hasher3d {};
template <typename T>
struct hash<Plane<T>> : PlaneHasher {};

} // namespace std

#endif // FRAC_POINT3D_HPP
