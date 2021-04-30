#include <bits/stdc++.h>

using namespace std;

#define long int64_t

// *****

inline namespace {

struct frac {
    long n, d;

    constexpr frac() : n(0), d(1) {}
    constexpr frac(long num) : n(num), d(1) {}
    constexpr frac(long num, long den) : n(num), d(den) {
        auto g = gcd(n, d);
        g = ((g < 0) == (d < 0)) ? g : -g;
        n /= g, d /= g;
    }

    explicit operator bool() const noexcept { return n != 0 && d != 0; }
    explicit operator long() const noexcept { return assert(d != 0), n / d; }
    explicit operator double() const noexcept { return assert(d != 0), 1.0 * n / d; }

    friend frac abs(frac f) { return frac(abs(f.n), f.d); }
    friend long floor(frac f) { return f.n >= 0 ? f.n / f.d : (f.n - f.d + 1) / f.d; }
    friend long ceil(frac f) { return f.n >= 0 ? (f.n + f.d - 1) / f.d : f.n / f.d; }
    friend frac gcd(frac a, frac b) {
        while (a != 0) {
            b = b % a;
            swap(a, b);
        }
        return abs(b);
    }

    friend bool operator==(frac a, frac b) { return a.n == b.n && a.d == b.d; }
    friend bool operator!=(frac a, frac b) { return a.n != b.n || a.d != b.d; }
    friend bool operator<(frac a, frac b) { return a.n * b.d < b.n * a.d; }
    friend bool operator>(frac a, frac b) { return a.n * b.d > b.n * a.d; }
    friend bool operator<=(frac a, frac b) { return a.n * b.d <= b.n * a.d; }
    friend bool operator>=(frac a, frac b) { return a.n * b.d >= b.n * a.d; }

    friend frac operator+(frac a, frac b) {
        return frac(a.n * b.d + b.n * a.d, a.d * b.d);
    }
    friend frac operator-(frac a, frac b) {
        return frac(a.n * b.d - b.n * a.d, a.d * b.d);
    }
    friend frac operator*(frac a, frac b) { return frac(a.n * b.n, a.d * b.d); }
    friend frac operator/(frac a, frac b) { return frac(a.n * b.d, a.d * b.n); }
    friend frac operator%(frac a, frac b) { return a - long(a / b) * b; }
    friend frac& operator+=(frac& a, frac b) { return a = a + b; }
    friend frac& operator-=(frac& a, frac b) { return a = a - b; }
    friend frac& operator*=(frac& a, frac b) { return a = a * b; }
    friend frac& operator/=(frac& a, frac b) { return a = a / b; }
    friend frac& operator%=(frac& a, frac b) { return a = a % b; }

    friend frac operator-(frac f) { return frac(-f.n, f.d); }
    friend bool operator!(frac f) { return f.n == 0; }

    friend string to_string(frac f) {
        if (f.d == 0) {
            return f.n ? f.n > 0 ? "inf" : "-inf" : "undef";
        } else if (f.d == 1) {
            return to_string(f.n);
        } else {
            return to_string(f.n) + '/' + to_string(f.d);
        }
    }

    friend ostream& operator<<(ostream& out, frac f) { return out << to_string(f); }
};

} // namespace
namespace std {

template <typename T>
string to_string(const vector<vector<T>>& v) {
    string s;
    for (const auto& el : v)
        s += "[" + to_string(el) + "] ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& v) {
    return out << to_string(v);
}

template <>
struct hash<frac> {
    size_t operator()(frac f) const noexcept {
        size_t a = hash<long>{}(f.n), b = hash<long>{}(f.d);
        return (a + b) * (a + b + 1) / 2 + b;
    }
};

template <typename U, typename V>
string to_string(const pair<U, V>& uv) {
    return '(' + to_string(uv.first) + ',' + to_string(uv.second) + ')';
}
template <typename U, typename V>
ostream& operator<<(ostream& out, const pair<U, V>& v) {
    return out << to_string(v);
}

template <typename T>
string to_string(const vector<T>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T>
ostream& operator<<(ostream& out, const vector<T>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const set<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const set<T, Rs...>& v) {
    return out << to_string(v);
}

} // namespace std

struct Point {
    frac x, y;
    int id;
    frac slope() const { return y / x; }
    frac lo() const { return y / (x + 1); }
    frac hi() const { return (y + 1) / x; }
};

#define MAXN 200'001
Point points[MAXN];
vector<pair<int, int>> removed_list;
vector<frac> slopeof;
unordered_map<frac, int> idof;
vector<set<pair<int, int>>> adj;
vector<vector<int>> assigned;

struct low_degree {
    bool operator()(int a, int b) const {
        return make_pair(adj[a].size(), a) < make_pair(adj[b].size(), b);
    }
};

#define POP(v, u, i) nodes.erase(v), adj[v].erase({u, i}), nodes.insert(v)

auto solve() {
    int N;
    cin >> N;
    adj.assign(2 * N, {});
    for (int i = 1; i <= N; i++) {
        long a, b, c, d;
        cin >> a >> b >> c >> d;
        frac x(a, b), y(c, d);
        points[i] = {frac(a, b), frac(c, d), i};
        int u = idof.emplace(points[i].lo(), idof.size()).first->second;
        int v = idof.emplace(points[i].hi(), idof.size()).first->second;
        assert(u != v);
        adj[u].insert({v, i});
        adj[v].insert({u, i});
    }
    int S = idof.size();
    assigned.assign(S, {});
    set<int, low_degree> nodes;

    for (int u = 0; u < S; u++) {
        nodes.insert(u);
    }

    while (!nodes.empty()) {
        int u = *nodes.begin();
        nodes.erase(u);

        if (assigned[u].size() % 2 == 1 && !adj[u].empty()) {
            auto [v, i] = *adj[u].begin();
            adj[u].erase({v, i});
            assigned[u].push_back(i);
            POP(v, u, i);
        }
        while (adj[u].size() >= 2u) {
            auto [v, i] = *adj[u].begin();
            adj[u].erase({v, i});
            auto [w, j] = *adj[u].begin();
            adj[u].erase({w, j});
            POP(v, u, i), POP(w, u, j);
            assigned[u].push_back(i);
            assigned[u].push_back(j);
        }
        if (adj[u].size() == 1u) {
            auto [v, i] = *adj[u].begin();
            adj[u].erase({v, i});
            assigned[v].push_back(i);
            POP(v, u, i);
        }
    }

    for (int u = 0; u < S; u++) {
        int n = assigned[u].size();
        for (int i = 0; i + 1 < n; i += 2) {
            removed_list.emplace_back(assigned[u][i], assigned[u][i + 1]);
        }
    }

    cout << removed_list.size() << endl;
    for (auto [a, b] : removed_list) {
        cout << a << ' ' << b << endl;
    }
}

// *****

int main() {
    ios::sync_with_stdio(false);
    solve();
    return 0;
}
