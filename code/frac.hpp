#ifndef FRAC_HPP
#define FRAC_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

// positive infinity is frac(1, 0) and negative infinity is frac(-1, 0)
struct frac {
    long n, d;

    constexpr frac() : n(0), d(1) {}
    constexpr frac(long num) : n(num), d(1) {}
    constexpr frac(long num, long den) : n(num), d(den) {
        if (d < 0) {
            n = -n, d = -d;
        }
        auto g = gcd(n, d);
        g = g < 0 ? -g : g;
        n /= g, d /= g;
    }

    explicit operator long() const noexcept {
        return d != 0 ? n / d : (n > 0 ? LONG_MAX : LONG_MIN);
    }
};

frac abs(frac f) { return frac(abs(f.n), f.d); }
long floor(frac f) { return f.n >= 0 ? f.n / f.d : (f.n - f.d + 1) / f.d; }
long ceil(frac f) { return f.n >= 0 ? (f.n + f.d - 1) / f.d : f.n / f.d; }

inline namespace frac_comparison {

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

} // namespace frac_comparison

inline namespace frac_arithmetic {

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

frac operator-(frac f) { return frac(-f.n, f.d); }
bool operator!(frac f) { return f.n == 0; }

} // namespace frac_arithmetic

string to_string(frac f) {
    if (f.d == 0) {
        return f.n ? f.n > 0 ? "inf" : "-inf" : "undef";
    } else if (f.d == 1) {
        return to_string(f.n);
    } else {
        return to_string(f.n) + '/' + to_string(f.d);
    }
}

ostream& operator<<(ostream& out, frac f) { return out << to_string(f); }

#endif // FRAC_HPP
