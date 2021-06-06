#include <bits/stdc++.h>

using namespace std;

// *****

long fracgcd(long a, long b) {
    while (a != 0) {
        b = b % a;
        swap(a, b);
    }
    return abs(b);
}

struct frac {
    long n, d;

    frac(long num) : n(num), d(1) {}
    frac(long num, long den) : n(num), d(den) {
        if (d < 0) {
            n = -n, d = -d;
        }
        long g = fracgcd(n, d);
        n /= g, d /= g;
    }

    operator long() const {
        return d != 0 ? n / d : (n > 0 ? LONG_MAX : LONG_MIN);
    }
};

frac inv(const frac& f) {
    return frac(f.d, f.n);
}

bool operator==(const frac& a, const frac& b) {
    return a.n == b.n && a.d == b.d;
}
bool operator!=(const frac& a, const frac& b) {
    return a.n != b.n || a.d != b.d;
}
bool operator<(const frac& a, const frac& b) {
    return a.n * b.d < b.n * a.d;
}
bool operator>(const frac& a, const frac& b) {
    return a.n * b.d > b.n * a.d;
}
bool operator<=(const frac& a, const frac& b) {
    return a.n * b.d <= b.n * a.d;
}
bool operator>=(const frac& a, const frac& b) {
    return a.n * b.d >= b.n * a.d;
}

frac operator+(const frac& a, const frac& b) {
    return frac(a.n * b.d + b.n * a.d, a.d * b.d);
}
frac operator-(const frac& a, const frac& b) {
    return frac(a.n * b.d - b.n * a.d, a.d * b.d);
}

ostream& operator<<(ostream& out, const frac& f) {
    return out << f.n << '/' << f.d;
}

#define MAXN 1000
#define MAXV 1'000'000'100L

int N;
long A[MAXN];
long B[MAXN];

frac cf_search(frac lo, frac hi) {
    assert(lo < hi);
    long a = lo;
    if (hi > frac(a + 1)) {
        return a + 1;
    }
    frac fa = frac(a);
    if (lo.d == 1) {
        return fa + inv(inv(hi - fa) + frac(1));
    } else {
        return fa + inv(cf_search(inv(hi - fa), inv(lo - fa)));
    }
}

auto solve() {
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> A[i] >> B[i];
    }
    frac lo(0, 1);
    frac hi(MAXV, 1);
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            long a = A[i], b = B[i];
            long x = A[j], y = B[j];
            // aC + bJ < xC + yJ
            if (a > x && y > b) {
                lo = max(lo, frac(a - x, y - b));
            } else if (a < x && y < b) {
                hi = min(hi, frac(x - a, b - y));
            } else if (a >= x && y <= b) {
                return "IMPOSSIBLE"s;
            }
        }
    }
    if (hi <= lo) {
        return "IMPOSSIBLE"s;
    }
    frac ans = cf_search(lo, hi);
    assert(lo < ans && ans < hi);
    return to_string(ans.d) + ' ' + to_string(ans.n);
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
