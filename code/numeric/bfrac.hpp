#pragma once

#include "bigint.hpp" // bigint

/**
 * Frac for rational arithmetic using bigint.
 * Positive infinity is frac(1, 0) and negative infinity is frac(-1, 0).
 */
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

    explicit operator bool() const noexcept { return n != 0 && d != 0; }
    explicit operator bigint() const noexcept { return assert(d != 0), n / d; }
    explicit operator double() const noexcept {
        return assert(d != 0), double(n) / double(d);
    }

    friend bfrac abs(const bfrac& f) { return bfrac(abs(f.n), f.d); }
    friend bigint floor(const bfrac& f) {
        return f.n >= 0 ? f.n / f.d : (f.n - f.d + 1) / f.d;
    }
    friend bigint ceil(const bfrac& f) {
        return f.n >= 0 ? (f.n + f.d - 1) / f.d : f.n / f.d;
    }
    friend bfrac gcd(const bfrac& a, const bfrac& b) { return a ? gcd(b, a) % a : b; }

    friend bool operator==(const bfrac& a, const bfrac& b) {
        return a.n == b.n && a.d == b.d;
    }
    friend bool operator!=(const bfrac& a, const bfrac& b) {
        return a.n != b.n || a.d != b.d;
    }
    friend bool operator<(const bfrac& a, const bfrac& b) {
        return a.n * b.d < b.n * a.d;
    }
    friend bool operator>(const bfrac& a, const bfrac& b) {
        return a.n * b.d > b.n * a.d;
    }
    friend bool operator<=(const bfrac& a, const bfrac& b) {
        return a.n * b.d <= b.n * a.d;
    }
    friend bool operator>=(const bfrac& a, const bfrac& b) {
        return a.n * b.d >= b.n * a.d;
    }

    friend bfrac operator+(const bfrac& a, const bfrac& b) {
        return bfrac(a.n * b.d + b.n * a.d, a.d * b.d);
    }
    friend bfrac operator-(const bfrac& a, const bfrac& b) {
        return bfrac(a.n * b.d - b.n * a.d, a.d * b.d);
    }
    friend bfrac operator*(const bfrac& a, const bfrac& b) {
        return bfrac(a.n * b.n, a.d * b.d);
    }
    friend bfrac operator/(const bfrac& a, const bfrac& b) {
        return bfrac(a.n * b.d, a.d * b.n);
    }
    friend bfrac operator%(const bfrac& a, const bfrac& b) {
        return a - bigint(a / b) * b;
    }
    friend bfrac& operator+=(bfrac& a, const bfrac& b) { return a = a + b; }
    friend bfrac& operator-=(bfrac& a, const bfrac& b) { return a = a - b; }
    friend bfrac& operator*=(bfrac& a, const bfrac& b) { return a = a * b; }
    friend bfrac& operator/=(bfrac& a, const bfrac& b) { return a = a / b; }
    friend bfrac& operator%=(bfrac& a, const bfrac& b) { return a = a % b; }

    friend bfrac operator-(const bfrac& f) { return bfrac(-f.n, f.d); }
    friend bool operator!(const bfrac& f) { return f.n == 0; }

    friend string to_string(const bfrac& f) {
        if (f.d == 0) {
            return f.n ? f.n > 0 ? "inf" : "-inf" : "undef";
        } else if (f.d == 1) {
            return to_string(f.n);
        } else {
            return to_string(f.n) + '/' + to_string(f.d);
        }
    }

    friend ostream& operator<<(ostream& out, const bfrac& f) {
        return out << to_string(f);
    }
};

inline namespace bfrac_format {

bfrac stobfrac(const string& s) {
    int i = 0, S = s.size();
    while (i < S && isspace(s[i]))
        i++; // skip whitespace
    int j = i;
    bool neg = j < S && s[j] == '-', pos = j < S && s[j] == '+';
    j += neg || pos; // skip leading sign
    bigint integer;
    while (j < S && '0' <= s[j] && s[j] <= '9') {
        integer = 10 * integer + (s[j] - '0'), j++; // read digits
    }
    integer = neg ? -integer : integer;
    if (i < j && j < S && s[j] == '/') {
        int n = 0, k = j + 1;
        bigint denom = 0;
        while (k < S && '0' <= s[k] && s[k] <= '9') {
            denom = 10 * denom + (s[k] - '0'), n++, k++; // read digits
        }
        return n ? bfrac(integer, denom) : bfrac(integer);
    } else if (j < S && s[j] == '.') {
        int n = 0, k = j + 1;
        bigint decimal = 0, ten = 1;
        while (k < S && '0' <= s[k] && s[k] <= '9') {
            decimal = 10 * decimal + (s[k] - '0'), ten *= 10, n++, k++; // read digits
        }
        decimal = neg ? -decimal : decimal;
        return bfrac(integer * ten + decimal, ten);
    } else {
        return integer;
    }
}

istream& operator>>(istream& in, bfrac& f) {
    string s;
    return in >> s, f = stobfrac(s), in;
}

} // namespace bfrac_format

namespace std {

template <>
struct hash<bfrac> {
    size_t operator()(const bfrac& f) const noexcept {
        size_t a = hash<bigint>{}(f.n), b = hash<bigint>{}(f.d);
        return (a + b) * (a + b + 1) / 2 + b;
    }
};

} // namespace std
