#include <bits/stdc++.h>

using namespace std;

inline namespace lib {

template <typename Seq>
string seq_to_string(const Seq& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}

template <typename Mat>
string mat_to_string(const Mat& v) {
    int N = v.size();
    vector<vector<string>> cell(1);
    vector<size_t> width(1);
    for (const auto& row : v) {
        int M = row.size(), i = cell.size();
        cell.emplace_back(M + 1, "");
        width.resize(max(int(width.size()), M + 1), 0);
        int j = 1;
        for (const auto& col : row) {
            if constexpr (std::is_same<decltype(col), const string&>::value) {
                cell[i][j] = col;
            } else {
                cell[i][j] = to_string(col);
            }
            width[j] = max(width[j], cell[i][j].size());
            j++;
        }
    }
    int M = width.size() - 1;
    cell[0].resize(M + 1);
    for (int i = 1; i <= N; i++) {
        cell[i][0] = to_string(i - 1);
        width[0] = max(width[0], cell[i][0].size());
    }
    for (int j = 1; j <= M; j++) {
        cell[0][j] = to_string(j - 1);
        width[j] = max(width[j], cell[0][j].size());
    }
    string s;
    for (int i = 0; i <= N; i++) {
        for (int S = cell[i].size(), j = 0; j < S; j++) {
            s += string(width[j] + 1 - cell[i][j].size(), ' ') + cell[i][j];
        }
        s += '\n';
    }
    return s;
}

} // namespace lib

namespace std {

template <typename U, typename V>
string to_string(const pair<U, V>& uv) {
    return '(' + to_string(uv.first) + ',' + to_string(uv.second) + ')';
}
template <typename U, typename V>
ostream& operator<<(ostream& out, const pair<U, V>& v) {
    return out << to_string(v);
}

template <typename T>
string to_string(const array<T, 2>& uv) {
    return '(' + to_string(uv[0]) + ',' + to_string(uv[1]) + ')';
}
template <typename T, size_t N>
ostream& operator<<(ostream& out, const array<T, N>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const vector<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const vector<T, Rs...>& v) {
    return out << to_string(v);
}

} // namespace std

inline namespace lib {

template <typename... Ts>
void debugger(string_view vars, Ts&&... args) {
    cout << ">> [" << vars << "]: ";
    const char* delim = "";
    (..., (cout << delim << args, delim = ", "));
    cout << endl;
}
#define debug(...) debugger(#__VA_ARGS__, __VA_ARGS__)

} // namespace lib

template <typename T, typename D = double>
struct Point2d {
    T x, y;
    Point2d() : x(0), y(0) {}
    Point2d(T x, T y) : x(x), y(y) {}
    Point2d(const array<T, 2>& arr) : x(arr[0]), y(arr[1]) {}
    Point2d(const pair<T, T>& p) : x(p.first), y(p.second) {}
    template <typename K>
    explicit Point2d(Point2d<K, D> other) : x(other.x), y(other.y) {}

    using P = Point2d<T, D>;
    using dP = Point2d<D, D>;
    static P zero() { return P(0, 0); }
    static T my_abs(T v) { return v >= 0 ? v : -v; /* __int128_t has no abs() */ }

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

    auto manh() const { return my_abs(x) + my_abs(y); }
    friend auto manh(P a, P b) { return (a - b).manh(); }
    friend auto abs(P u) { return P(my_abs(u.x), my_abs(u.y)); }

    friend auto dot(P u, P v) { return u.x * v.x + u.y * v.y; }
    auto doted(P a, P b) const { return dot(a - *this, b - *this); }

    friend auto cross(P u, P v) { return u.x * v.y - u.y * v.x; }
    auto crossed(P a, P b) const { return cross(a - *this, b - *this); }

    friend auto dot_cross(P u, P v) { return P(dot(u, v), cross(u, v)); }
    auto dot_crossed(P a, P b) const { return dot_cross(a - *this, b - *this); }

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
    friend D signed_linedist(P a, P u, P v) { return D(a.cross(u, v)) / dist(u, v); }
    friend D linedist(P a, P u, P v) { return abs(signed_linedist(a, u, v)); }

  public: // Format
    friend string to_string(const P& p) {
        return '(' + to_string(p.x) + ',' + to_string(p.y) + ')';
    }
    friend ostream& operator<<(ostream& out, const P& p) { return out << to_string(p); }
    friend istream& operator>>(istream& in, P& p) { return in >> p.x >> p.y; }
};

template <typename P>
auto graham_scan(vector<P> pts) {
    int N = pts.size();
    auto lo = pts[0];
    for (const auto& point : pts) {
        if (make_pair(point[1], point[0]) < make_pair(lo[1], lo[0])) {
            lo = point;
        }
    }

    vector<int> index(N);
    iota(begin(index), end(index), 0);
    sort(begin(index), end(index), [&](int u, int v) {
        P lhs = pts[u], rhs = pts[v];
        auto cross = lo.crossed(lhs, rhs);
        return make_pair(cross, rhs[1]) > make_pair(decltype(cross)(0), lhs[1]);
    });

    int s = 0;
    vector<int> hull = {index[0], index[1]};
    for (int i = 2; i < N; i++) {
        debug(s, i, N, hull);
        while (pts[hull[s]].crossed(pts[hull[s + 1]], pts[index[i]]) < 0) {
            hull.pop_back(), s--;
            assert(s >= 0);
        }
        hull.push_back(index[i]), s++;
    }
    return hull;
}

using P = Point2d<long>;
using edges_t = vector<array<int, 2>>;
using hull_t = vector<int>;
int N;
vector<P> pts;

vector<int> ordered(int& a, int& b, int& c) {
    if (a.crossed(b, c) < 0)
        return {a, c, b};
    else
        return {a, b, c};
}

bool inside(P a, P b, P c, P p) {
    if (a.crossed(b, c) > 0) {
        return a.crossed(b, p) > 0 && b.crossed(c, p) > 0 && c.crossed(a, p) > 0;
    } else {
        return a.crossed(c, p) > 0 && c.crossed(b, p) > 0 && b.crossed(a, p) > 0;
    }
}

bool inside(int a, int b, int c, int d) { return inside(pts[a], pts[b], pts[c], pts[d]); }

vector<int> all_inside(int a, int b, int c) {
    vector<int> got;
    for (int i = 1; i <= N; i++) {
        if (inside(a, b, c, i)) {
            got.push_back(i);
        }
    }
    return got;
}

edges_t triangulate(hull_t hull, vector<int> inside) {}

auto solve() {
    int N;
    cin >> N;
    pts.resize(N + 1);
    for (int i = 1; i <= N; i++) {
        cin >> pts[i].x >> pts[i].y;
    }
    int a, b, c, d;
    cin >> a >> b >> c >> d;

    edges_t intedges;
    hull_t inth;

    bool equal = set<int>({a, b, c, d}).size();

    if (equal) {
        if (pts[a].crossed(pts[c], pts[d]) > 0) {
            inth = {a, c, d};
            intedges = triangulate(inth, all_inside(inth[0], inth[1], inth[2]));
        } else {
            inth = {a, d, c};
            intedges = triangulate(inth, all_inside(inth[0], inth[1], inth[2]));
        }
    } else {
        vector<array<int, 4>> orderings = {{a, b, c, d},
                                           {a, b, d, c},
                                           {a, c, d, b},
                                           {b, c, d, a}};
        bool found = false;
        for (auto [x, y, z, w] : orderings) {
            if (inside(x, y, z, w)) {
                auto it0 = ordered(x, y, w);
                auto it1 = ordered(y, z, w);
                auto it2 = ordered(z, x, w);
                auto h0 = triangulate(it0, all_inside(it0[0], it0[1], it0[2]));
                auto h1 = triangulate(it1, all_inside(it1[0], it1[1], it1[2]));
                auto h2 = triangulate(it2, all_inside(it2[0], it2[1], it2[2]));
                intedges.insert(end(intedges), h0);
                intedges.insert(end(intedges), h1);
                intedges.insert(end(intedges), h2);
                inth = ordered(x, y, z);
                found = true;
                break;
            }
        }
        if (!found) {
            if (cross(pts[a], pts[b], pts[c]) < 0) {
                swap(a, b);
            }
            if (cross(pts[a], pts[c], pts[d]) < 0) {
                swap(c, d);
            }
            auto ins = all_inside(a, b, c);
            ins.insert(end(ins), all_inside(c, d, a));
            inth = {a, b, c, d};
            intedges = triangulate(inth, ins);
        }
    }

    auto positive_pts = pts;
    positive_pts.erase(begin(positive_pts));
    auto hull = graham_scan(positive_pts);
    for (int& i : hull)
        i++;

    vector<int> interior_hull;
    if ()

        debug(hull);

    auto by_x = [&](int u, int v) {
        return make_pair(pts[u].x, pts[u].y) < make_pair(pts[v].x, pts[v].y);
    };
    auto by_x = [&](int u, int v) {
        return make_pair(pts[u].y, pts[u].x) < make_pair(pts[v].y, pts[v].x);
    };

    set<int> hull_set(begin(hull), end(hull));

    auto hull_by_x = hull;
    sort(begin(hull_by_x), end(hull_by_x), by_x);

    set<int> frontier;
    frontier.insert(hull_by_x[0]);
    frontier.insert(hull_by_x[1]);
    frontier.insert(hull_by_x[2]);

    auto pts_by_x = pts;
    sort(begin(pts_by_x), end(pts_by_x), by_x);

    for (int i = 0; i < N; i++) {
        int x =
    }

    return 0;
}

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
