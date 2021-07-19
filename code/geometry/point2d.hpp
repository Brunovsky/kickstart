#pragma once

#include "../hash.hpp"

template <typename T, typename D = double>
struct Point2d {
    T x, y;
    // int id = -1;

    Point2d() : x(0), y(0) {}
    Point2d(T x, T y) : x(x), y(y) {}
    // Point2d(T x, T y, int id) : x(x), y(y), id(id) {}
    template <typename K>
    explicit Point2d(Point2d<K, D> other) : x(other.x), y(other.y) {}

    using P = Point2d<T, D>;
    static T my_abs(T v) { return v >= 0 ? v : -v; /* __int128_t has no abs() */ }

    friend bool operator==(P a, P b) { return a.x == b.x && a.y == b.y; }
    friend bool operator!=(P a, P b) { return !(a == b); }
    explicit operator bool() const noexcept { return x || y; }
    bool boxed(P min, P max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y;
    }
    friend P clamp(P p, P min, P max) {
        return P(clamp(p.x, min.x, max.x), clamp(p.y, min.y, max.y));
    }

    T& operator[](int i) { return assert(i == 0 || i == 1), *(&x + i); }
    T operator[](int i) const { return assert(i == 0 || i == 1), *(&x + i); }
    P operator-() const { return P(-x, -y); }
    P operator+() const { return P(x, y); }
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

    friend auto norm2(P u) { return u.x * u.x + u.y * u.y; }
    friend auto norm(P u) { return std::sqrt(D(norm2(u))); }
    friend auto unit(P u) { return Point2d<D, D>(u) / norm(u); }

    friend auto manh(P u) { return my_abs(u.x) + my_abs(u.y); }
    friend auto manh(P a, P b) { return manh(a - b); }
    friend auto abs(P u) { return P(my_abs(u.x), my_abs(u.y)); }

    friend auto dot(P u, P v) { return u.x * v.x + u.y * v.y; }
    auto doted(P a, P b) const { return dot(a - *this, b - *this); }

    friend auto cross(P u, P v) { return u.x * v.y - u.y * v.x; }
    auto crossed(P a, P b) const { return cross(a - *this, b - *this); }

    friend auto dot_cross(P u, P v) { return P(dot(u, v), cross(u, v)); }
    auto dot_crossed(P a, P b) const { return dot_cross(a - *this, b - *this); }

    friend auto dist2(P a, P b) { return norm2(a - b); }
    friend auto dist(P a, P b) { return std::sqrt(D(dist2(a, b))); }

    friend P interpolate(P a, P b, T k) { return a + (b - a) * k; }

  public: // Angles (many from ecnerwala/kactl)
    // multiplies the result's norm by abs(u)
    auto rotate(P u) const { return dot_cross(conj(u), *this); }
    auto unrotate(P u) const { return dot_cross(u, *this); }

    friend int quadrant(P p) {
        return p.x >= 0 && p.y >= 0  ? 0
               : p.x <= 0 && p.y > 0 ? 1
               : p.x < 0 && p.y <= 0 ? 2
                                     : 3;
    }

    // operator<(u,v) for angles, ccw, prefers smaller distances
    friend bool angle_less(P u, P v) {
        return quadrant(u) == quadrant(v)
                   ? cross(u, v) == 0 ? norm2(u) < norm2(v) : cross(u, v) > 0
                   : quadrant(u) < quadrant(v);
    }

    // operator<(u,v) for angles as seen from pivot, ccw
    friend bool angle_less(P pivot, P u, P v) { return angle_less(u - pivot, v - pivot); }

    // is p inside angle <(u,v) ccw? 1=in, 0=online, -1=outside
    friend int angle_between(P p, P u, P v) {
        return samedir(p, u) || samedir(p, v) ? 0 : angle_less(u, p, v) ? 1 : -1;
    }

    friend D angle(P u) { return atan2(D(u.y), D(u.x)); }
    friend D cos(P u) { return clamp(D(u.x) / norm(u), D(-1), D(1)); }
    friend D sin(P u) { return clamp(D(u.y) / norm(u), D(-1), D(1)); }
    friend D angle(P u, P v) { return acos(cos(u, v)); }
    friend D cos(P u, P v) {
        return clamp(D(dot(u, v)) / (norm(u) * norm(v)), D(-1), D(1));
    }
    friend D sin(P u, P v) {
        return clamp(D(cross(u, v)) / (norm(u) * norm(v)), D(-1), D(1));
    }

  public: // Lines
    friend bool parallel(P u, P v) { return cross(u, v) == 0; }
    friend bool samedir(P u, P v) { return cross(u, v) == 0 && dot(u, v) >= 0; }
    friend bool collinear(P a, P b, P c) { return a.crossed(b, c) == 0; }
    friend bool onsegment(P a, P b, P c) {
        return a.crossed(b, c) == 0 && b.doted(a, c) <= 0;
    }

    friend int lineside(P p, P u, P v) {
        auto c = u.crossed(v, p);
        return (c >= 0) - (c <= 0);
    }
    friend optional<P> intersect(P a, P b, P u, P v) {
        auto d = cross(v - u, b - a), p = a.crossed(v, b), q = a.crossed(b, u);
        return d == 0 ? std::nullopt : (u * p + v * q) / d;
    }

    friend T linedist2(P a, P u, P v) {
        auto c = a.crossed(u, v);
        return c * c / dist2(u, v);
    }
    friend D signed_linedist(P a, P u, P v) { return D(a.crossed(u, v)) / dist(u, v); }
    friend D linedist(P a, P u, P v) { return abs(signed_linedist(a, u, v)); }

  public: // Area
    template <typename K = T>
    friend K area(P a, P b, P c) {
        return K(a.crossed(b, c)) / K(2);
    }
    template <typename K = T>
    friend K area(const vector<P>& ps) {
        K ans = 0;
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

namespace std {

template <typename T, typename D>
struct hash<Point2d<T, D>> {
    size_t operator()(const Point2d<T, D>& p) const noexcept {
        array<T, 2> arr{p[0], p[1]};
        return std::hash<array<T, 2>>{}(arr);
    }
};

} // namespace std
