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

frac absdif(const frac& a, const frac& b) {
    return frac(abs(a.n * b.d - b.n * a.d), a.d * b.d);
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
