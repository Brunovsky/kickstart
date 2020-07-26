#include <bits/stdc++.h>

using namespace std;

// *****

struct frac {
    long n, d;

    frac(long num) : n(num), d(1) {}
    frac(long num, long den) : n(num), d(den) {
        if (d < 0) {
            n = -n, d = -d;
        }
        long g = gcd(n, d);
        n /= g, d /= g;
    }

    operator long() const {
        return d != 0 ? n / d : (n > 0 ? LONG_MAX : LONG_MIN);
    }

private:
    static long gcd(long a, long b) {
        while (a != 0) {
            b = b % a;
            swap(a, b);
        }
        return abs(b);
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

// fraction closest to f with denominator at most maxd
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

using cf_t = vector<long>;

// the first continued fraction representation of f
cf_t cf1_sequence(frac f) {
    cf_t cf1;
    while (f.n) {
        swap(f.n, f.d);
        cf1.push_back(f.n / f.d);
        f.n = f.n % f.d;
    }
    return cf1;
}

// the second continued fraction representation of f:  a] --> a-1,a]
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
    return f;
}

// from codejam: compute fraction between lo and hi with smallest possible denominator
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
