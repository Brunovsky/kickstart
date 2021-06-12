#pragma once

#include "../hash.hpp"

/**
 * Class to represent points exactly in 3d space.
 * Set T to long, frac, __int128_t, double, long double, ... (avoid int due to overflow)
 * Set D to double or long double.
 * Careful with exact comparisons like u==v if T is not exact.
 * The implementation in epsilon_point3d.hpp handles doubles properly with epsilons etc.
 *
 * Points: a,b,c,p   Points/vectors: u,v,w
 * Primary sources: kactl, cp-alg, ecnerwala
 */
template <typename T, typename D = double>
struct Point3d {
    T x, y, z;
    // int id = -1;

    Point3d() : x(0), y(0), z(0) {}
    Point3d(T x, T y, T z) : x(x), y(y), z(z) {}
    template <typename K>
    explicit Point3d(Point3d<K, D> other) : x(other.x), y(other.y), z(other.z) {}

    using P = Point3d<T, D>;
    using dP = Point3d<D, D>;
    static P zero() { return P(0, 0, 0); }
    static T my_abs(T v) { return v >= 0 ? v : -v; /* __int128_t has no abs() */ }

    friend bool operator==(P a, P b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
    friend bool operator!=(P a, P b) { return !(a == b); }
    explicit operator bool() const noexcept { return *this != zero(); }
    bool boxed(P min, P max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y && min.z <= z &&
               z <= max.z;
    }
    friend P clamp(P p, P min, P max) {
        return P(clamp(p.x, min.x, max.x), clamp(p.y, min.y, max.y),
                 clamp(p.z, min.z, max.z));
    }

    T& operator[](int i) { return assert(i >= 0 && i <= 2), *(&x + i); }
    T operator[](int i) const { return assert(i >= 0 && i <= 2), *(&x + i); }
    P operator-() const { return P(-x, -y, -z); }
    friend P operator+(P u, P v) { return P(u.x + v.x, u.y + v.y, u.z + v.z); }
    friend P operator-(P u, P v) { return P(u.x - v.x, u.y - v.y, u.z - v.z); }
    friend P operator*(T k, P u) { return P(u.x * k, u.y * k, u.z * k); }
    friend P operator*(P u, T k) { return P(u.x * k, u.y * k, u.z * k); }
    friend P operator/(P u, T k) { return P(u.x / k, u.y / k, u.z / k); }
    friend P& operator+=(P& u, P v) { return u = u + v; }
    friend P& operator-=(P& u, P v) { return u = u - v; }
    friend P& operator*=(P& u, T k) { return u = u * k; }
    friend P& operator/=(P& u, T k) { return u = u / k; }

    // auto int_norm() const { return gcd(x, gcd(y, z)); }
    // auto int_unit() const { return *this ? *this / int_norm() : *this; }
    // friend auto int_norm(P p) { return p.int_norm(); }
    // friend auto int_unit(P p) { return p.int_unit(); }

    auto norm2() const { return x * x + y * y + z * z; }
    auto norm() const { return std::sqrt(D(norm2())); }
    auto unit() const { return dP(*this) / norm(); }
    friend auto norm(P u) { return u.norm(); }
    friend auto unit(P u) { return u.unit(); }

    auto manh() const { return my_abs(x) + my_abs(y) + my_abs(z); }
    friend auto manh(P a, P b) { return (a - b).manh(); }
    friend auto abs(P u) { return P(my_abs(u.x), my_abs(u.y), my_abs(u.z)); }

    friend auto dot(P u, P v) { return u.x * v.x + u.y * v.y + u.z * v.z; }
    auto doted(P a, P b) const { return dot(a - *this, b - *this); }

    friend auto cross(P u, P v) {
        return P(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
    }
    auto crossed(P a, P b) const { return cross(a - *this, b - *this); }

    friend auto dist2(P a, P b) { return (a - b).norm2(); }
    friend auto dist(P a, P b) { return std::sqrt(D(dist2(a, b))); }

  public: // Angles
    friend D angle(P u, P v) { return acos(cos(u, v)); }
    friend D cos(P u, P v) {
        return clamp(D(dot(u, v)) / (u.norm() * v.norm()), D(-1), D(1));
    }
    friend D sin(P u, P v) {
        return clamp(D(cross(u, v)) / (u.norm() * v.norm()), D(-1), D(1));
    }

  public: // Lines
    friend P interpolate(P a, P b, T k) { return a + (b - a) * k; }

    friend bool parallel(P u, P v) { return cross(u, v) == zero(); }
    friend bool samedir(P u, P v) { return cross(u, v) == zero() && dot(u, v) > 0; }
    friend bool collinear(P a, P b, P c) { return a.crossed(b, c) == zero(); }
    friend bool onsegment(P a, P b, P c) {
        return a.crossed(b, c) == zero() && b.doted(a, c) < 0;
    }

    friend T linedist2(P a, P u, P v) { return a.crossed(u, v).norm2() / dist2(u, v); }
    friend auto signed_linedist(P a, P u, P v) {
        return a.crossed(u, v).norm() / dist(u, v);
    }
    friend auto linedist(P a, P u, P v) { return abs(signed_linedist(a, u, v)); }

  public: // Planes
    friend bool coplanar(P a, P b, P c, P d) {
        return parallel(a.crossed(c, d), b.crossed(c, d));
    }
    friend int planeside(P p, P a, P b, P c) { return planeside(p, a, a.crossed(b, c)); }
    friend int planeside(P p, P c, P n) {
        auto s = dot(n, p - c);
        return (s >= 0) - (s <= 0);
    }

    friend T planedist2(P p, P c, P n) {
        auto k = dot(n, p - c);
        return k * k / n.norm2();
    }
    friend auto signed_planedist(P p, P c, P n) { return D(dot(n, p - c)) / n.norm(); }
    friend auto planedist(P p, P c, P n) { return abs(signed_planedist(p, c, n)); }

  public: // Format
    friend string to_string(const P& p) {
        return '(' + to_string(p.x) + ',' + to_string(p.y) + ',' + to_string(p.z) + ')';
    }
    friend ostream& operator<<(ostream& out, P p) { return out << to_string(p); }
    friend istream& operator>>(istream& in, P& p) { return in >> p.x >> p.y >> p.z; }
};

template <typename T, typename D = double>
struct Plane {
    using P = Point3d<T, D>;
    P a, b, c;

    Plane() = default;
    Plane(P a, P b, P c) : a(a), b(b), c(c) {}

    P normal() const { return a.crossed(b, c); }
    bool is_degenerate() const { return collinear(a, b, c); }
    friend bool same_oriented(Plane f, Plane g) {
        return samedir(f.normal(), g.normal()) && coplanar(f.a, f.b, f.c, g.a);
    }
    friend bool operator==(Plane f, Plane g) { return same_oriented(f, g); }
    friend bool operator!=(Plane f, Plane g) { return !(f == g); }

    Plane operator-() const { return Plane(a, c, b); }

    int planeside(P p) const {
        auto s = dot(normal(), p - c);
        return (s >= 0) - (s <= 0);
    }
    T planedist2(P p) {
        auto k = dot(normal(), p - c);
        return k * k / normal().norm2();
    }
    auto signed_planedist(P p) { return D(dot(normal(), p - c)) / normal().norm(); }
    auto planedist(P p) { return abs(signed_planedist(p)); }
};

namespace std {

template <typename T, typename D>
struct hash<Point3d<T, D>> {
    size_t operator()(const Point3d<T, D>& p) const noexcept {
        array<T, 3> arr{p[0], p[1], p[2]};
        return std::hash<array<T, 3>>{}(arr);
    }
};
template <typename T, typename D>
struct hash<Plane<T, D>> {
    size_t operator()(const Plane<T, D>& plane) const noexcept {
        array<Point3d<T, D>, 3> arr{plane.a, plane.b, plane.c};
        return std::hash<array<Point3d<T, D>, 3>>(arr);
    }
};

} // namespace std
