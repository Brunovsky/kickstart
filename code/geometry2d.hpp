#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Class to represent points, lines, axis, circles etc in 2d.
 * Real coordinates
 * Primary source: kth (https://github.com/kth-competitive-programming/kactl)
 */
struct Point;
using P = Point;

static constexpr double eps = 1e-14;
static constexpr double inf = DBL_MAX;

struct Point {
    double x, y;
    Point() : x(0), y(0) {}
    Point(double x, double y) : x(x), y(y) {}
    explicit Point(pair<double, double> p) : x(p.first), y(p.second) {}
    explicit Point(array<double, 2> a) : x(a[0]), y(a[1]) {}

    bool iszero() const { return x * x + y * y <= eps * eps; }
    bool isinf() const { return x >= inf || y >= inf; }
    double angle() const { return atan2(y, x); }
    double cross(P a, P b) { return crossed(a - *this, b - *this); }
    P& rotate(double a) { return *this = rotated(*this, a); }
    P& rotate(double a, P axis) { return *this = rotated(*this, a, axis); }

    friend P operator-(P a) { return P(-a.x, -a.y); }
    friend P operator+(P a, P b) { return P(a.x + b.x, a.y + b.y); }
    friend P operator-(P a, P b) { return P(a.x - b.x, a.y - b.y); }
    friend P operator*(double k, P a) { return P(k * a.x, k * a.y); }
    friend P operator*(P a, double k) { return P(k * a.x, k * a.y); }
    friend P operator/(P a, double k) { return P(a.x / k, a.y / k); }
    friend P& operator+=(P& a, P b) { return a = a + b; }
    friend P& operator-=(P& a, P b) { return a = a - b; }
    friend P& operator*=(P& a, double k) { return a = a * k; }
    friend P& operator/=(P& a, double k) { return a = a / k; }
    friend bool operator==(P a, P b) { return (a - b).iszero(); }
    friend bool operator!=(P a, P b) { return !(a == b); }
    friend bool operator<(P a, P b) { return tie(a.x, a.y) < tie(b.x, b.y); }
    friend bool operator>(P a, P b) { return b < a; }
    friend bool operator<=(P a, P b) { return !(b < a); }
    friend bool operator>=(P a, P b) { return !(a < b); }
    friend double dist2(P a) { return a.x * a.x + a.y * a.y; }
    friend double dist2(P a, P b) { return dist2(a - b); }
    friend double dist(P a) { return sqrt(dist2(a)); }
    friend double dist(P a, P b) { return sqrt(dist2(a, b)); }
    friend double dot(P a, P b) { return a.x * b.x + a.y * b.y; }
    friend double angle(P a) { return atan2(a.y, a.x); }
    friend double angle(P a, P b) { return acos(dot(a, b) / (dist(a) * dist(b))); }
    friend double crossed(P a, P b) { return a.x * b.y - a.y * b.x; }
    friend P unit(P a) { return a / dist(a); }
    friend P perp(P a) { return P(-a.y, a.x); }
    friend P normal(P a) { return unit(perp(a)); }
    friend P rotated(P p, double a, P axis) { return (p - axis).rotate(a) + axis; }
    friend P rotated(P p, double a) {
        return P(p.x * cos(a) - p.y * sin(a), p.x * sin(a) + p.y * cos(a));
    }

    bool boxed(P min, P max) const {
        return min.x <= x && x <= max.x && min.y <= y && y <= max.y;
    }
};

string to_string(P a) { return '(' + to_string(a.x) + ',' + to_string(a.y) + ')'; }
ostream& operator<<(ostream& out, const P& a) { return out << to_string(a); }

struct point_hasher {
    size_t operator()(P p) const noexcept {
        using std::hash;
        hash<double> hasher;
        size_t seed = hasher(p.x) + 0x9e3779b9;
        seed ^= hasher(p.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

// are three points collinear (in any order)?
bool collinear(P a, P b, P c) { return abs(a.cross(b, c)) <= eps; }

// are three points collinear (in that order)?
bool onsegment(P a, P b, P c) { return collinear(a, b, c) && dot(a - b, c - b) <= 0; }

// distance from p to uv
double linedist(P p, P u, P v) { return u.cross(p, v) / dist(u, v); }

// distance from p to [uv]
double segmentdist(P p, P u, P v) {
    if (u == v)
        return dist(u, p);
    auto d = dist2(u, v), t = min(d, max(.0, dot(p - u, v - u)));
    return dist((p - u) * d, (v - u) * t) / d;
}

// two tangents to a circle
vector<pair<P, P>> tangents(P c1, double r1, P c2, double r2) {
    P d = c2 - c1;
    double dr = r1 - r2, d2 = dist2(d), h2 = d2 - dr * dr;
    if (abs(d2) <= eps || h2 < 0)
        return {};
    vector<pair<P, P>> out;
    for (double sign : {-1, 1}) {
        P v = (d * dr + perp(d) * sqrt(h2) * sign) / d2;
        out.push_back({c1 + v * r1, c2 + v * r2});
    }
    if (abs(h2) <= eps)
        out.pop_back();
    return out;
}

// intersect two lines uv, ab
P intersect(P u, P v, P a, P b) {
    auto d = crossed(v - u, b - a);
    if (abs(d) <= eps)
        return P(inf, inf);
    auto p = a.cross(v, b), q = a.cross(b, u);
    return (u * p + v * q) / d;
}

// intersect two circles
vector<P> intersect(P c1, double r1, P c2, double r2) {
    if (c1 == c2) {
        assert(abs(r1 - r2) > eps);
        return {};
    }
    P vec = c2 - c1;
    double d2 = dist2(vec);
    double sum = r1 + r2, dif = r1 - r2;
    double p = (d2 + r1 * r1 - r2 * r2) / (d2 * 2);
    double h2 = r1 * r1 - p * p * d2;
    if (sum * sum < d2 || dif * dif > d2)
        return {};
    P mid = c1 + vec * p, per = perp(vec) * sqrt(fmax(0.0, h2) / d2);
    return {mid + per, mid - per};
}

// intersect a line and a circle
vector<P> intersect(P c, double r, P a, P b) {
    P ab = b - a, p = a + ab * dot(ab, c - a) / dist2(ab);
    double s = a.cross(b, c);
    double h2 = r * r - s * s / dist2(ab);
    if (h2 < 0)
        return {};
    if (abs(h2) <= eps)
        return {p};
    P h = unit(ab) * sqrt(h2);
    return {p - h, p + h};
}

double area(P A, P B, P C) { return abs(A.cross(B, C)) / 2; }

double circumradius(P A, P B, P C) {
    return dist(A, B) * dist(B, C) * dist(C, A) / (2 * abs(A.cross(B, C)));
}

P circumcenter(P A, P B, P C) {
    P ac = C - A, ab = B - A;
    return A + perp(ac * dist2(ab) - ab * dist2(ac)) / (2 * crossed(ac, ab));
}

double inradius(P A, P B, P C) {
    double a = dist(B, C), b = dist(C, A), c = dist(A, B);
    return abs(A.cross(B, C)) / (a + b + c);
}

P incenter(P A, P B, P C) {
    double a = dist(B, C), b = dist(C, A), c = dist(A, B);
    return (a * A + b * B + c * C) / (a + b + c);
}

P orthocenter(P A, P B, P C) {
    P b = perp(A - C), c = perp(A - B);
    return intersect(B, B + b, C, C + c);
}
