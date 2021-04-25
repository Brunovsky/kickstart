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
        auto g = gcd(n, d);
        g = ((g < 0) == (d < 0)) ? g : -g;
        n /= g, d /= g;
    }

    explicit operator long() const noexcept { return assert(d != 0), n / d; }
    explicit operator bool() const noexcept { return n != 0 && d != 0; }
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

} // namespace frac_comparison

inline namespace frac_arithmetic {

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

inline namespace frac_format {

frac stofrac(const string& s) {
    int i = 0, S = s.size();
    while (i < S && isspace(s[i]))
        i++; // skip whitespace
    int j = i;
    bool neg = j < S && s[j] == '-', pos = j < S && s[j] == '+';
    j += neg || pos; // skip leading sign
    long integer = 0;
    while (j < S && '0' <= s[j] && s[j] <= '9') {
        integer = 10 * integer + (s[j] - '0'), j++; // read digits
    }
    integer = neg ? -integer : integer;
    if (i < j && j < S && s[j] == '/') {
        int n = 0, k = j + 1;
        long denom = 0;
        while (k < S && '0' <= s[k] && s[k] <= '9') {
            denom = 10 * denom + (s[k] - '0'), n++, k++; // read digits
        }
        return n ? frac(integer, denom) : frac(integer);
    } else if (j < S && s[j] == '.') {
        int n = 0, k = j + 1;
        long decimal = 0, ten = 1;
        while (k < S && '0' <= s[k] && s[k] <= '9') {
            decimal = 10 * decimal + (s[k] - '0'), ten *= 10, n++, k++; // read digits
        }
        decimal = neg ? -decimal : decimal;
        return frac(integer * ten + decimal, ten);
    } else {
        return integer;
    }
}

istream& operator>>(istream& in, frac& f) {
    string s;
    return in >> s, f = stofrac(s), in;
}

double fractod(frac f) { return assert(f.d != 0), 1.0 * f.n / f.d; }
double to_decimal(frac f) { return fractod(f); }

} // namespace frac_format

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

namespace std {

template <>
struct hash<frac> {
    size_t operator()(frac f) const noexcept {
        size_t a = hash<long>{}(f.n), b = hash<long>{}(f.d);
        return (a + b) * (a + b + 1) / 2 + b;
    }
};

} // namespace std

#endif // FRAC_HPP