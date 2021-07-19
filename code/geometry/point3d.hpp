#pragma once

#include "../hash.hpp"

template <typename T, typename D = double>
struct Point3d {
    T x, y, z;
    // int id = -1;

    Point3d() : x(0), y(0), z(0) {}
    Point3d(T x, T y, T z) : x(x), y(y), z(z) {}
    // Point3d(T x, T y, T z, int id) : x(x), y(y), z(z), id(id) {}
    template <typename K>
    explicit Point3d(Point3d<K, D> other) : x(other.x), y(other.y), z(other.z) {}

    using P = Point3d<T, D>;
    static T my_abs(T v) { return v >= 0 ? v : -v; /* __int128_t has no abs() */ }

    friend bool operator==(P a, P b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
    friend bool operator!=(P a, P b) { return !(a == b); }
    explicit operator bool() const noexcept { return x || y || z; }
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
    P operator+() const { return P(x, y, z); }
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

    friend auto norm2(P u) { return u.x * u.x + u.y * u.y + u.z * u.z; }
    friend auto norm(P u) { return std::sqrt(D(norm2(u))); }
    friend auto unit(P u) { return Point3d<D, D>(u) / norm(u); }

    friend auto manh(P u) { return my_abs(u.x) + my_abs(u.y) + my_abs(u.z); }
    friend auto manh(P a, P b) { return manh(a - b); }
    friend auto abs(P u) { return P(my_abs(u.x), my_abs(u.y), my_abs(u.z)); }

    friend auto dot(P u, P v) { return u.x * v.x + u.y * v.y + u.z * v.z; }
    auto doted(P a, P b) const { return dot(a - *this, b - *this); }

    friend auto cross(P u, P v) {
        return P(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
    }
    auto crossed(P a, P b) const { return cross(a - *this, b - *this); }

    friend auto dist2(P a, P b) { return norm2(a - b); }
    friend auto dist(P a, P b) { return std::sqrt(D(dist2(a, b))); }

    friend P interpolate(P a, P b, T k) { return a + (b - a) * k; }

  public: // Angles
    friend D angle(P u, P v) { return acos(cos(u, v)); }
    friend D cos(P u, P v) {
        return clamp(D(dot(u, v)) / (norm(u) * norm(v)), D(-1), D(1));
    }
    friend D sin(P u, P v) {
        return clamp(D(cross(u, v)) / (norm(u) * norm(v)), D(-1), D(1));
    }

  public: // Lines
    friend bool parallel(P u, P v) { return cross(u, v) == P(); }
    friend bool samedir(P u, P v) { return cross(u, v) == P() && dot(u, v) > 0; }
    friend bool collinear(P a, P b, P c) { return a.crossed(b, c) == P(); }
    friend bool onsegment(P a, P b, P c) {
        return a.crossed(b, c) == P() && b.doted(a, c) < 0;
    }

    friend T linedist2(P a, P u, P v) { return norm2(a.crossed(u, v)) / dist2(u, v); }
    friend auto signed_linedist(P a, P u, P v) {
        return norm(a.crossed(u, v)) / dist(u, v);
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
        return k * k / norm2(n);
    }
    friend auto signed_planedist(P p, P c, P n) { return D(dot(n, p - c)) / norm(n); }
    friend auto planedist(P p, P c, P n) { return abs(signed_planedist(p, c, n)); }

  public: // Format
    friend string to_string(P p) {
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
        return k * k / norm2(normal());
    }
    auto signed_planedist(P p) { return D(dot(normal(), p - c)) / norm(normal()); }
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
