#ifndef BFRAC_HPP
#define BFRAC_HPP

#include "bigint.hpp" // bigint

// *****

// positive infinity is bfrac(1, 0) and negative infinity is bfrac(-1, 0)
struct bfrac {
    bigint n, d;

    bfrac() : n(0), d(1) {}
    bfrac(int num) : n(num), d(1) {}
    bfrac(int num, int den) : bfrac(bigint(num), bigint(den)) {}
    bfrac(bigint num) : n(move(num)), d(1) {}
    bfrac(bigint num, bigint den) : n(move(num)), d(move(den)) {
        auto g = gcd(n, d);
        g = ((g < 0) == (d < 0)) ? g : -g;
        n /= g, d /= g;
    }

    explicit operator bigint() const noexcept { return assert(d != 0), n / d; }
    explicit operator bool() const noexcept { return n != 0 && d != 0; }
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

} // namespace bfrac_comparison

inline namespace bfrac_arithmetic {

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
