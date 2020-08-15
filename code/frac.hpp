#include <bits/stdc++.h>

using namespace std;

// *****

// positive infinity is frac(1, 0) and negative infinity is frac(-1, 0)
struct frac {
    long n, d;

    constexpr frac() : n(0), d(1) {}
    constexpr frac(long num) : n(num), d(1) {}
    frac(long num, long den) : n(num), d(den) {
        if (d < 0) {
            n = -n, d = -d;
        }
        long g = abs(__gcd(n, d));
        n /= g, d /= g;
    }

    explicit operator long() const noexcept {
        return d != 0 ? n / d : (n > 0 ? LONG_MAX : LONG_MIN);
    }
};

frac inv(frac f) { return f.n >= 0 ? frac(f.d, f.n) : frac(-f.d, -f.n); }
frac abs(frac f) { return frac(abs(f.n), f.d); }
frac operator-(frac f) { return frac(-f.n, f.d); }
bool operator!(frac f) { return f.n == 0 ? 1L : 0L; }

bool operator==(frac a, frac b) { return a.n == b.n && a.d == b.d; }
bool operator!=(frac a, frac b) { return a.n != b.n || a.d != b.d; }
bool operator<(frac a, frac b) { return a.n * b.d < b.n * a.d; }
bool operator>(frac a, frac b) { return a.n * b.d > b.n * a.d; }
bool operator<=(frac a, frac b) { return a.n * b.d <= b.n * a.d; }
bool operator>=(frac a, frac b) { return a.n * b.d >= b.n * a.d; }
bool operator==(frac a, long b) { return a.n == b && a.d == 1; }
bool operator!=(frac a, long b) { return a.n != b || a.d != 1; }
bool operator<(frac a, long b) { return a.n < b * a.d; }
bool operator>(frac a, long b) { return a.n > b * a.d; }
bool operator<=(frac a, long b) { return a.n <= b * a.d; }
bool operator>=(frac a, long b) { return a.n >= b * a.d; }
bool operator==(long b, frac a) { return a.n == b && a.d == 1; }
bool operator!=(long b, frac a) { return a.n != b || a.d != 1; }
bool operator<(long b, frac a) { return b * a.d < a.n; }
bool operator>(long b, frac a) { return b * a.d > a.n; }
bool operator<=(long b, frac a) { return b * a.d <= a.n; }
bool operator>=(long b, frac a) { return b * a.d >= a.n; }

frac operator+(frac a, long b) { return frac(a.n + b * a.d, a.d); }
frac operator-(frac a, long b) { return frac(a.n - b * a.d, a.d); }
frac operator*(frac a, long b) { return frac(a.n * b, a.d); }
frac operator/(frac a, long b) { return frac(a.n, a.d * b); }
frac operator%(frac a, long b) { return a - b * long(a / b); }
frac operator+(long b, frac a) { return frac(b * a.d + a.n, a.d); }
frac operator-(long b, frac a) { return frac(b * a.d - a.n, a.d); }
frac operator*(long b, frac a) { return frac(b * a.n, a.d); }
frac operator/(long b, frac a) { return frac(b * a.d, a.n); }
frac operator%(long b, frac a) { return b - long(b / a) * a; }
frac& operator+=(frac& a, long b) { return a = a + b; }
frac& operator-=(frac& a, long b) { return a = a - b; }
frac& operator*=(frac& a, long b) { return a = a * b; }
frac& operator/=(frac& a, long b) { return a = a / b; }
frac& operator%=(frac& a, long b) { return a = a % b; }

frac operator+(frac a, frac b) { return frac(a.n * b.d + b.n * a.d, a.d * b.d); }
frac operator-(frac a, frac b) { return frac(a.n * b.d - b.n * a.d, a.d * b.d); }
frac operator*(frac a, frac b) { return frac(a.n * b.n, a.d * b.d); }
frac operator/(frac a, frac b) { return frac(a.n * b.d, a.d * b.n); }
frac operator%(frac a, frac b) { return a - long(a / b) * b; }
frac& operator+=(frac& a, frac b) { return a = a + b; }
frac& operator-=(frac& a, frac b) { return a = a - b; }
frac& operator*=(frac& a, frac b) { return a = a * b; }
frac& operator/=(frac& a, frac b) { return a = a / b; }
frac& operator%=(frac& a, frac b) { return a = a % b; }

frac operator""_f(unsigned long long n) { return frac(n); }
frac operator""_n(unsigned long long n) { return frac(n, 1); }
frac operator""_d(unsigned long long n) { return frac(1, n); }
long floor(frac f) {
    if (f.d == 0) {
        return long(f);
    }
    return f.n >= 0 ? f.n / f.d : (f.n - f.d + 1) / f.d;
}
long ceil(frac f) {
    if (f.d == 0) {
        return long(f);
    }
    return f.n >= 0 ? (f.n + f.d - 1) / f.d : f.n / f.d;
}
string to_string(frac f) {
    if (f.d == 0) {
        return f.n > 0 ? "inf" : "-inf";
    } else if (f.d == 1) {
        return to_string(f.n);
    } else {
        return to_string(f.n) + '/' + to_string(f.d);
    }
}
ostream& operator<<(ostream& out, const frac& f) { return out << to_string(f); }

// fraction closest to f with denominator at most maxd
frac closest(frac f, long maxd) {
    long p0 = 0, q0 = 1;
    long p1 = 1, q1 = 0;
    long n = f.n, d = f.d;
    if (d <= maxd) {
        return f;
    }
    while (true) {
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
    return abs(b1 - f) <= abs(b2 - f) ? b1 : b2;
}

using cf_t = vector<long>;

// the first continued fraction representation of f
cf_t cf1_sequence(frac f) {
    cf_t cf1;
    cf1.push_back(floor(f));
    f = f - floor(f);
    while (f.n) {
        swap(f.n, f.d);
        cf1.push_back(f.n / f.d);
        f.n = f.n % f.d;
    }
    return cf1;
}

// the second continued fraction representation of f:  a] --> a-1,1]
cf_t cf2_sequence(frac f) {
    cf_t cf2 = cf1_sequence(f);
    if (!cf2.empty()) {
        cf2.back()--;
        cf2.push_back(1);
    }
    return cf2;
}

// compute continued fraction representation
frac compute_fraction(const cf_t& cf) {
    frac f(0, 1);
    for (int i = cf.size() - 1; i >= 0; i--) {
        f.n += cf[i] * f.d;
        swap(f.n, f.d);
    }
    return inv(f);
}

// from codejam: compute fraction between lo and hi with smallest possible denominator
frac cf_search(frac lo, frac hi) {
    assert(lo < hi);
    long a = floor(lo);
    if (hi > a + 1) {
        return a + 1;
    }
    frac fa = frac(a);
    if (lo.d == 1) {
        return fa + inv(inv(hi - fa) + 1);
    } else {
        return fa + inv(cf_search(inv(hi - fa), inv(lo - fa)));
    }
}
