#ifndef POINT2D_HPP
#define POINT2D_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Class to represent points exactly in 2d space.
 * Set T to long, __int128_t, frac, double, long double, ... (avoid int due to overflow)
 * Set D to double or long double.
 * Don't untemplate it or you will get compile errors.
 * Careful with exact comparisons like u==v if T is not exact
 * Points: a,b,c,p   Points/vectors: u,v,w
 */
template <typename T, typename D = double>
struct Point2d {
    T x, y;
    Point2d() : x(0), y(0) {}
    Point2d(T x, T y) : x(x), y(y) {}
    Point2d(const pair<T, T>& p) : x(p.first), y(p.second) {}
    Point2d(const array<T, 2>& arr) : x(arr[0]), y(arr[1]) {}
    template <typename K>
    explicit Point2d(Point2d<K, D> other) : x(other.x), y(other.y) {}

    using P = Point2d<T, D>;
    using dP = Point2d<D, D>;
    static P zero() { return P(0, 0); }

    friend bool operator==(P a, P b) { return a.x == b.x && a.y == b.y; }
    friend bool operator!=(P a, P b) { return !(a == b); }
    explicit operator bool() const noexcept { return *this != zero(); }
    bool boxed(P min, P max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y;
    }
    friend P clamp(P p, P min, P max) {
        return P(clamp(p.x, min.x, max.x), clamp(p.y, min.y, max.y));
    }

    T& operator[](int i) { return assert(i == 0 || i == 1), *(&x + i); }
    T operator[](int i) const { return assert(i == 0 || i == 1), *(&x + i); }
    P operator-() const { return P(-x, -y); }
    friend P operator+(P u, P v) { return P(u.x + v.x, u.y + v.y); }
    friend P operator-(P u, P v) { return P(u.x - v.x, u.y - v.y); }
    friend P operator*(T k, P u) { return P(u.x * k, u.y * k); }
    friend P operator*(P u, T k) { return P(u.x * k, u.y * k); }
    friend P operator/(P u, T k) { return P(u.x / k, u.y / k); }
    friend P& operator+=(P& u, P v) { return u = u + v; }
    friend P& operator-=(P& u, P v) { return u = u - v; }
    friend P& operator*=(P& u, T k) { return u = u * k; }
    friend P& operator/=(P& u, T k) { return u = u / k; }

    friend auto conj(P u) { return P(u.x, -u.y); } // complex conjugate
    friend auto perp_ccw(P u) { return P(-u.y, u.x); }
    friend auto perp_cw(P u) { return P(u.y, -u.x); }

    // auto int_norm() const { return gcd(x, y); }
    // auto int_unit() const { return *this ? *this / int_norm() : *this; }
    // friend auto int_norm(P p) { return p.int_norm(); }
    // friend auto int_unit(P p) { return p.int_unit(); }

    auto norm2() const { return x * x + y * y; }
    auto norm() const { return std::sqrt(D(norm2())); }
    auto unit() const { return dP(*this) / norm(); }
    friend auto norm(P u) { return u.norm(); }
    friend auto unit(P u) { return u.unit(); }

    auto manh() const { return abs(x) + abs(y); }
    friend auto manh(P a, P b) { return (a - b).manh(); }
    friend auto abs(P u) { return P(abs(u.x), abs(u.y)); }

    friend auto dot(P u, P v) { return u.x * v.x + u.y * v.y; }
    friend auto doted(P p, P a, P b) { return dot(a - p, b - p); }

    friend auto cross(P u, P v) { return u.x * v.y - u.y * v.x; }
    friend auto crossed(P p, P a, P b) { return cross(a - p, b - p); }

    friend auto dot_cross(P u, P v) { return P(dot(u, v), cross(u, v)); }
    friend auto dot_crossed(P p, P a, P b) { return dot_cross(a - p, b - p); }

    friend auto dist2(P a, P b) { return (a - b).norm2(); }
    friend auto dist(P a, P b) { return std::sqrt(D(dist2(a, b))); }

  public: // Angles (many from ecnerwala/kactl)
    // multiplies the result's norm by abs(u)
    auto rotate(P u) const { return dot_cross(conj(u), *this); }
    auto unrotate(P u) const { return dot_cross(u, *this); }

    // going ccw from u to v is <=180º
    friend bool less_pi(P u, P v) {
        auto c = cross(u, v);
        return c ? (c > 0) : (dot(u, v) > 0);
    }

    // operator<(u,v) for angles as seen from base ccw
    friend bool angle_less(P base, P u, P v) {
        int r = less_pi(base, v) - less_pi(base, u);
        return r ? (r < 0) : (cross(u, v) > 0);
    }

    // is p inside angle <(u,v) ccw? 1=in, 0=online, -1=outside
    friend int angle_between(P p, P u, P v) {
        return samedir(p, u) || samedir(p, v) ? 0 : angle_less(u, p, v) ? 1 : -1;
    }

    friend D angle(P u) { return atan2(D(u.y), D(u.x)); }
    friend D cos(P u) { return clamp(D(u.x) / u.norm(), D(-1), D(1)); }
    friend D sin(P u) { return clamp(D(u.y) / u.norm(), D(-1), D(1)); }
    friend D angle(P u, P v) { return acos(cos(u, v)); }
    friend D cos(P u, P v) {
        return clamp(D(dot(u, v)) / (u.norm() * v.norm()), D(-1), D(1));
    }
    friend D sin(P u, P v) {
        return clamp(D(cross(u, v)) / (u.norm() * v.norm()), D(-1), D(1));
    }

  public: // Lines
    friend P interpolate(P a, P b, T k) { return a + (b - a) * k; }

    friend bool parallel(P u, P v) { return cross(u, v) == 0; }
    friend bool samedir(P u, P v) { return cross(u, v) == 0 && dot(u, v) >= 0; }
    friend bool collinear(P a, P b, P c) { return a.crossed(b, c) == 0; }
    friend bool onsegment(P a, P b, P c) {
        return a.crossed(b, c) == 0 && doted(b, a, c) <= 0;
    }

    friend int lineside(P p, P u, P v) {
        auto c = u.crossed(v, p);
        return (c >= 0) - (c <= 0);
    }
    friend optional<P> intersect(P a, P b, P u, P v) {
        auto d = cross(v - u, b - a), p = a.cross(v, b), q = a.cross(b, u);
        return d == 0 ? std::nullopt : (u * p + v * q) / d;
    }

    friend T linedist2(P a, P u, P v) {
        auto c = a.crossed(u, v);
        return c * c / dist2(u, v);
    }
    friend D signed_linedist(P a, P u, P v) { return D(a.cross(u, v)) / dist(u, v); }
    friend D linedist(P a, P u, P v) { return abs(signed_linedist(a, u, v)); }

  public: // Format
    friend string to_string(const P& p) {
        return '(' + to_string(p.x) + ',' + to_string(p.y) + ')';
    }
    friend ostream& operator<<(ostream& out, const P& p) { return out << to_string(p); }
    friend istream& operator>>(istream& in, P& p) { return in >> p.x >> p.y; }
};

#endif // POINT2D_HPP
