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
    frac(long num, long den) : n(num), d(den) {
        if (d < 0) {
            n = -n, d = -d;
        }
        long g = fracgcd(n, d);
        n /= g, d /= g;
    }
};

bool operator==(const frac& a, const frac& b) {
    return a.n == b.n && a.d == b.d;
}
bool operator!=(const frac& a, const frac& b) {
    return a.n != b.n || a.d != b.d;
}
bool operator<(const frac& a, const frac& b) {
    return 1.0 * a.n / a.d < 1.0 * b.n / b.d;
}
bool operator>(const frac& a, const frac& b) {
    return 1.0 * a.n / a.d > 1.0 * b.n / b.d;
}
bool operator<=(const frac& a, const frac& b) {
    return 1.0 * a.n / a.d <= 1.0 * b.n / b.d;
}
bool operator>=(const frac& a, const frac& b) {
    return 1.0 * a.n / a.d >= 1.0 * b.n / b.d;
}

frac operator+(const frac& a, const frac& b) {
    return frac(a.n * b.d + b.n * a.d, a.d * b.d);
}
frac operator-(const frac& a, const frac& b) {
    return frac(a.n * b.d - b.n * a.d, a.d * b.d);
}
frac operator*(const frac& a, const frac& b) {
    return frac(a.n * b.n, a.d * b.d);
}
frac operator/(const frac& a, const frac& b) {
    return frac(a.n * b.d, a.d * b.n);
}
frac operator%(const frac& a, const frac& b) {
    return frac((a.n * b.d) % (b.n * a.d), a.d * b.d);
}

double absdif(const frac& a, const frac& b) {
    return abs(1.0 * a.n / a.d - 1.0 * b.n / b.d);
}

ostream& operator<<(ostream& out, const frac& f) {
    return out << f.n << '/' << f.d;
}

frac closest(frac f, long maxd) {
    long p0 = 0, q0 = 1;
    long p1 = 1, q1 = 0;
    long n = f.n, d = f.d;
    if (d <= maxd) {
        return f;
    }
    while (1) {
        long a = n / d;
        long p2 = p0 + a * p1;
        long q2 = q0 + a * q1;
        if (q2 > maxd) {
            break;
        }
        p0 = p1, q0 = q1;
        p1 = p2, q1 = q2;
        n = n - a * d;
        swap(n, d);
    }
    long k = (maxd - q0) / q1;
    frac b1(p0 + k * p1, q0 + k * q1);
    frac b2(p1, q1);
    return absdif(b1, f) <= absdif(b2, f) ? b1 : b2;
}

#define MAXN 1000
#define MAXV 1'000'000'100LL

int N;
long A[MAXN];
long B[MAXN];

frac binary_search(frac lo, frac hi) {
    long a = lo.n, b = lo.d;
    long c = hi.n, d = hi.d;
    frac mid(a * d + b * c, 2 * b * d);
    // find x,y such that a/b < x/y < c/d
    long Ly = 1, Ry = b + d;
    while (Ly <= Ry) {
        long y = (Ly + Ry) / 2;
        frac close = closest(mid, y);
        if (lo < close && close < hi) {
            Ry = y - 1;
        } else {
            Ly = y + 1;
        }
    }
    long x = a * Ly / b + 1;
    return frac(x, Ly);
}

auto solve() {
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> A[i] >> B[i];
    }
    frac lo(1, MAXV);
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
    frac ans = binary_search(lo, hi);
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
