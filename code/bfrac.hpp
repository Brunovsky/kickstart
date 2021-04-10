#ifndef BFRAC_HPP
#define BFRAC_HPP

#include "bigint.hpp" // bigint

// *****

// positive infinity is bfrac(1, 0) and negative infinity is bfrac(-1, 0)
struct bfrac {
    bigint n, d;

    bfrac() : n(0), d(1) {}
    bfrac(bigint num) : n(num), d(1) {}
    bfrac(bigint num, bigint den) : n(num), d(den) {
        if (d < 0) {
            n = -n, d = -d;
        }
        auto g = abs(gcd(n, d));
        n /= g, d /= g;
    }

    explicit operator bigint() const noexcept { return assert(d != 0), n / d; }
};

bfrac abs(const bfrac& f) { return bfrac(abs(f.n), f.d); }
bigint floor(const bfrac& f) { return f.n >= 0 ? f.n / f.d : (f.n - f.d + 1) / f.d; }
bigint ceil(const bfrac& f) { return f.n >= 0 ? (f.n + f.d - 1) / f.d : f.n / f.d; }

inline namespace bfrac_comparison {

bool operator==(const bfrac& a, const bfrac& b) { return a.n == b.n && a.d == b.d; }
bool operator!=(const bfrac& a, const bfrac& b) { return a.n != b.n || a.d != b.d; }
bool operator<(const bfrac& a, const bfrac& b) { return a.n * b.d < b.n * a.d; }
bool operator>(const bfrac& a, const bfrac& b) { return a.n * b.d > b.n * a.d; }
bool operator<=(const bfrac& a, const bfrac& b) { return a.n * b.d <= b.n * a.d; }
bool operator>=(const bfrac& a, const bfrac& b) { return a.n * b.d >= b.n * a.d; }
bool operator==(const bfrac& a, const bigint& b) { return a.n == b && a.d == 1; }
bool operator!=(const bfrac& a, const bigint& b) { return a.n != b || a.d != 1; }
bool operator<(const bfrac& a, const bigint& b) { return a.n < b * a.d; }
bool operator>(const bfrac& a, const bigint& b) { return a.n > b * a.d; }
bool operator<=(const bfrac& a, const bigint& b) { return a.n <= b * a.d; }
bool operator>=(const bfrac& a, const bigint& b) { return a.n >= b * a.d; }
bool operator==(const bigint& b, const bfrac& a) { return a.n == b && a.d == 1; }
bool operator!=(const bigint& b, const bfrac& a) { return a.n != b || a.d != 1; }
bool operator<(const bigint& b, const bfrac& a) { return b * a.d < a.n; }
bool operator>(const bigint& b, const bfrac& a) { return b * a.d > a.n; }
bool operator<=(const bigint& b, const bfrac& a) { return b * a.d <= a.n; }
bool operator>=(const bigint& b, const bfrac& a) { return b * a.d >= a.n; }

} // namespace bfrac_comparison

inline namespace bfrac_arithmetic {

bfrac operator+(const bfrac& a, const bigint& b) { return bfrac(a.n + b * a.d, a.d); }
bfrac operator-(const bfrac& a, const bigint& b) { return bfrac(a.n - b * a.d, a.d); }
bfrac operator*(const bfrac& a, const bigint& b) { return bfrac(a.n * b, a.d); }
bfrac operator/(const bfrac& a, const bigint& b) { return bfrac(a.n, a.d * b); }
bfrac operator%(const bfrac& a, const bigint& b) { return a - b * bigint(a / b); }
bfrac operator+(const bigint& b, const bfrac& a) { return bfrac(b * a.d + a.n, a.d); }
bfrac operator-(const bigint& b, const bfrac& a) { return bfrac(b * a.d - a.n, a.d); }
bfrac operator*(const bigint& b, const bfrac& a) { return bfrac(b * a.n, a.d); }
bfrac operator/(const bigint& b, const bfrac& a) { return bfrac(b * a.d, a.n); }
bfrac operator%(const bigint& b, const bfrac& a) { return b - bigint(b / a) * a; }
bfrac& operator+=(bfrac& a, const bigint& b) { return a = a + b; }
bfrac& operator-=(bfrac& a, const bigint& b) { return a = a - b; }
bfrac& operator*=(bfrac& a, const bigint& b) { return a = a * b; }
bfrac& operator/=(bfrac& a, const bigint& b) { return a = a / b; }
bfrac& operator%=(bfrac& a, const bigint& b) { return a = a % b; }

bfrac operator+(const bfrac& a, const bfrac& b) {
    return bfrac(a.n * b.d + b.n * a.d, a.d * b.d);
}
bfrac operator-(const bfrac& a, const bfrac& b) {
    return bfrac(a.n * b.d - b.n * a.d, a.d * b.d);
}
bfrac operator*(const bfrac& a, const bfrac& b) { return bfrac(a.n * b.n, a.d * b.d); }
bfrac operator/(const bfrac& a, const bfrac& b) { return bfrac(a.n * b.d, a.d * b.n); }
bfrac operator%(const bfrac& a, const bfrac& b) { return a - bigint(a / b) * b; }
bfrac& operator+=(bfrac& a, const bfrac& b) { return a = a + b; }
bfrac& operator-=(bfrac& a, const bfrac& b) { return a = a - b; }
bfrac& operator*=(bfrac& a, const bfrac& b) { return a = a * b; }
bfrac& operator/=(bfrac& a, const bfrac& b) { return a = a / b; }
bfrac& operator%=(bfrac& a, const bfrac& b) { return a = a % b; }

bfrac operator-(const bfrac& f) { return bfrac(-f.n, f.d); }
bool operator!(const bfrac& f) { return f.n == 0; }

} // namespace bfrac_arithmetic

string to_string(const bfrac& f) {
    if (f.d == 0) {
        return f.n ? f.n > 0 ? "inf" : "-inf" : "undef";
    } else if (f.d == 1) {
        return to_string(f.n);
    } else {
        return to_string(f.n) + '/' + to_string(f.d);
    }
}

ostream& operator<<(ostream& out, const bfrac& f) { return out << to_string(f); }

#endif // BFRAC_HPP
