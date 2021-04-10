#include <bits/stdc++.h>

using namespace std;

// *****

namespace {

static_assert(0xffffffff == UINT_MAX);
static_assert(sizeof(uint) == 4 && sizeof(ulong) == 8, "Unexpected integer sizes");

struct bigint {
    vector<uint> nums;
    bool sign = 0; // 0=positive, 1=negative

    bigint() = default;
    bigint(int n) : nums(n != 0, abs(n)), sign(n < 0) {}
    bigint(uint n, bool s = 0) : nums(n > 0, n), sign(s) {}
    bigint(const string& s, uint b = 10);

    explicit operator bool() const { return zero(); }
    auto& operator[](uint x) { return nums[x]; }
    const auto& operator[](uint x) const { return nums[x]; }
    bool bit(uint x) const { return nums[x / 32] & (1 << (x % 32)); }
    int len() const { return nums.size(); }
    bool zero() const { return nums.empty(); }
    void clear() { nums.clear(), sign = 0; }
    void flip() { sign = !sign && !zero(); }
    void trim() {
        while (!zero() && nums.back() == 0)
            nums.pop_back();
        sign = sign && !zero();
    }
};

inline namespace bigint_comparison {

bool magnitude_cmp(const bigint& u, const bigint& v) {
    int L = u.len(), R = v.len();
    return L != R ? L < R
                  : lexicographical_compare(rbegin(u.nums), rend(u.nums), //
                                            rbegin(v.nums), rend(v.nums));
}

bool operator<(const bigint& u, const bigint& v) {
    return u.sign != v.sign ? u.sign : u.sign ? magnitude_cmp(v, u) : magnitude_cmp(u, v);
}
bool operator>(const bigint& u, const bigint& v) { return v < u; }
bool operator<=(const bigint& u, const bigint& v) { return !(u > v); }
bool operator>=(const bigint& u, const bigint& v) { return !(u < v); }
bool operator==(const bigint& u, const bigint& v) {
    return u.sign == v.sign && u.nums == v.nums;
}
bool operator!=(const bigint& u, const bigint& v) { return !(u == v); }

bool operator<(const bigint& u, int v) { return u < bigint(v); }
bool operator>(const bigint& u, int v) { return u > bigint(v); }
bool operator<=(const bigint& u, int v) { return u <= bigint(v); }
bool operator>=(const bigint& u, int v) { return u >= bigint(v); }
bool operator==(const bigint& u, int v) { return u == bigint(v); }
bool operator!=(const bigint& u, int v) { return u != bigint(v); }

bool operator<(int u, const bigint& v) { return bigint(u) < v; }
bool operator>(int u, const bigint& v) { return bigint(u) > v; }
bool operator<=(int u, const bigint& v) { return bigint(u) <= v; }
bool operator>=(int u, const bigint& v) { return bigint(u) >= v; }
bool operator==(int u, const bigint& v) { return bigint(u) == v; }
bool operator!=(int u, const bigint& v) { return bigint(u) != v; }

} // namespace bigint_comparison

inline namespace bigint_bitwise {

bigint& operator>>=(bigint& u, uint shift) {
    int s = shift / 32, n = u.len();
    uint lo = shift % 32, hi = 32 - lo;

    if (s >= n) {
        u.clear();
    } else if (lo > 0) {
        for (int i = 0; i < n - s - 1; i++)
            u[i] = (u[i + s] >> lo) | (u[i + s + 1] << hi);
        u[n - s - 1] = u[n - 1] >> lo;
        u.nums.resize(n - s);
        u.trim();
    } else {
        u.nums.erase(begin(u.nums), begin(u.nums) + s);
    }

    return u;
}

bigint& operator<<=(bigint& u, uint shift) {
    int s = shift / 32, n = u.len();
    uint hi = shift % 32, lo = 32 - hi;

    if (hi > 0) {
        u.nums.resize(n + s + 1, 0);
        for (int i = n + s; i > s; i--)
            u[i] = (u[i - s - 1] >> lo) | (u[i - s] << hi);
        u[s] = u[0] << hi;
        for (int i = s - 1; i >= 0; i--)
            u[i] = 0;
        u.trim();
    } else {
        u.nums.insert(begin(u.nums), s, 0);
    }

    return u;
}

bigint operator>>(bigint u, uint shift) { return u >>= shift; }
bigint operator<<(bigint u, uint shift) { return u <<= shift; }

bigint& operator&=(bigint& u, const bigint& v) {
    int n = min(u.len(), v.len());
    u.nums.resize(n);
    for (int i = 0; i < n; i++)
        u[i] = u[i] & v[i];
    u.trim();
    return u;
}
bigint& operator|=(bigint& u, const bigint& v) {
    int n = max(u.len(), v.len());
    u.nums.resize(n, 0);
    for (int i = 0; i < v.len(); i++)
        u[i] = u[i] | v[i];
    return u;
}
bigint& operator^=(bigint& u, const bigint& v) {
    int n = max(u.len(), v.len());
    u.nums.resize(n, 0);
    for (int i = 0; i < v.len(); i++)
        u[i] = u[i] ^ v[i];
    u.trim();
    return u;
}
bigint operator~(bigint u) {
    for (int i = 0; i < u.len(); i++)
        u[i] = ~u[i];
    u.trim();
    return u;
}

bigint operator&(bigint u, const bigint& v) { return u &= v; }
bigint operator|(bigint u, const bigint& v) { return u |= v; }
bigint operator^(bigint u, const bigint& v) { return u ^= v; }

string lsbits(const bigint& u) {
    if (u.zero())
        return "0";
    string s(32 * u.len() + 1, '0');
    s[0] = u.sign ? '-' : '+';
    for (int i = 0; i < 32 * u.len(); i++)
        s[i + 1] = '0' + u.bit(i);
    while (!s.empty() && s.back() == '0')
        s.pop_back();
    return s;
}

string msbits(const bigint& u) {
    if (u.zero())
        return "0";
    string s(32 * u.len() + 1, '0');
    s[0] = u.sign ? '-' : '+';
    for (int i = 0; i < 32 * u.len(); i++)
        s[32 * u.len() - i] = '0' + u.bit(i);
    s.erase(begin(s) + 1, find(begin(s) + 1, end(s), '1'));
    return s;
}

} // namespace bigint_bitwise

inline namespace bigint_routines {

void add_int(bigint& u, uint v) {
    for (int i = 0; v && i < u.len(); i++)
        u[i] += v, v = u[i] < v;
    if (v > 0)
        u.nums.push_back(v);
}

void sub_int(bigint& u, uint v) {
    if (v == 0)
        return;
    if (u.zero()) {
        u.nums = {v}, u.sign = 1;
        return;
    }
    if (u.len() == 1 && u[0] < v) {
        u.nums = {v - u[0]}, u.flip();
        return;
    }
    for (int i = 0; v && i < u.len(); i++) {
        bool carry = v > u[i];
        u[i] -= v;
        v = carry;
    }
    u.trim();
}

void mul_int(bigint& u, uint v) {
    if (v == 0) {
        u.clear();
        return;
    }
    if (v == 1) {
        return;
    }
    ulong m = v, sum = 0;
    for (int i = 0; i < u.len(); i++) {
        sum += u[i] * m;
        u[i] = sum;
        sum >>= 32;
    }
    if (sum > 0)
        u.nums.push_back(sum);
}

uint div_int(bigint& u, uint v) {
    constexpr ulong b = 1UL + UINT_MAX;
    assert(v > 0);
    if (v == 1 || u.zero())
        return 0;
    uint r = 0;
    for (int i = u.len() - 1; i >= 0; i--) {
        ulong p = r * b + u[i];
        u[i] = p / v, r = p % v;
    }
    u.trim();
    return r;
}

void add_vec(bigint& u, const bigint& v) {
    int n = u.len(), m = v.len(), hi = max(n, m), lo = min(n, m);
    u.nums.resize(hi, 0);
    ulong k = 0;
    for (int i = 0; i < lo; i++) {
        k += u[i], k += v[i];
        u[i] = k, k = k > UINT_MAX;
    }
    for (int i = lo; i < m; i++) {
        k += v[i];
        u[i] = k, k = k > UINT_MAX;
    }
    for (int i = m; k && i < hi; i++) {
        u[i]++, k = u[i] == 0;
    }
    if (k)
        u.nums.push_back(k);
}

void sub_vec(bigint& u, const bigint& v) {
    int n = u.len(), m = v.len();
    assert(n >= m);
    long k = 0;
    for (int i = 0; i < m; i++) {
        long sum = u[i] - k - v[i];
        u[i] = sum + UINT_MAX + 1;
        k = sum < 0;
    }
    for (int i = m; k && i < n; i++) {
        k = u[i] == 0;
        u[i]--;
    }
    assert(k == 0);
    u.trim();
}

void rev_sub_vec(bigint& u, const bigint& v) {
    int n = u.len(), m = v.len();
    assert(n <= m);
    u.nums.resize(m, 0);
    long k = 0;
    for (int i = 0; i < n; i++) {
        long sum = v[i] - k - u[i];
        u[i] = sum + UINT_MAX + 1;
        k = sum < 0;
    }
    for (int i = n; i < m; i++) {
        u[i] = v[i];
    }
    for (int i = n; k && i < m; i++) {
        k = v[i] == 0;
        u[i]--;
    }
    assert(k == 0);
    u.trim();
}

void dyn_sub_vec(bigint& u, const bigint& v) {
    int n = u.len(), m = v.len();
    if (n > m) {
        return sub_vec(u, v);
    } else if (n < m) {
        return rev_sub_vec(u, v), u.flip();
    } else {
        int i = n - 1;
        while (i >= 0 && u[i] == v[i])
            u.nums.pop_back(), i--;
        if (i < 0) {
            u.sign = 0;
            return;
        }
        n = i + 1;
        long k = 0;
        if (u[i] > v[i]) {
            for (i = 0; i < n; i++) {
                long sum = u[i] - k - v[i];
                u[i] = sum + UINT_MAX + 1;
                k = sum < 0;
            }
        } else {
            for (i = 0; i < n; i++) {
                long sum = v[i] - k - u[i];
                u[i] = sum + UINT_MAX + 1;
                k = sum < 0;
            }
            u.flip();
        }
        u.trim();
        assert(k == 0);
    }
}

bigint mul_vec(const bigint& u, const bigint& v) {
    if (u.zero() || v.zero())
        return 0;
    int n = u.len(), m = v.len();
    bigint c;
    c.nums.resize(n + m, 0);
    c.sign = u.sign ^ v.sign;
    for (int j = 0; j < m; j++) {
        uint k = 0;
        for (int i = 0; i < n; i++) {
            ulong t = ulong(u[i]) * v[j] + c[i + j] + k;
            c[i + j] = t & UINT_MAX;
            k = t >> 32;
        }
        c[n + j] = k;
    }
    c.trim();
    return c;
}

bigint div_vec(bigint& u, bigint v) {
    constexpr ulong b = 1L + UINT_MAX;
    assert(!v.zero());

    // return the remainder and set u to the quotient, but throughout the algorithm
    // u is the remainder and d is the quotient.
    int n = v.len(), m = u.len() - n;
    uint c = __builtin_clz(v[n - 1]);
    u <<= c, v <<= c;
    if (u.len() == n + m)
        u.nums.push_back(0);
    assert(u.len() == n + m + 1 && v.len() == n && v[n - 1] >= b / 2);

    bigint d;
    d.nums.resize(m + 1, 0);
    d.sign = u.sign ^ v.sign;

    for (int j = m; j >= 0; j--) {
        ulong q = (u[n + j] * b + u[n - 1 + j]) / v[n - 1];
        ulong r = (u[n + j] * b + u[n - 1 + j]) % v[n - 1];

        while (r < b && q * v[n - 2] > r * b + u[n - 2 + j])
            q--, r += v[n - 1];
        if (q == 0)
            continue;

        long k = 0, t;
        for (int i = 0; i < n; i++) {
            ulong p = q * v[i];
            t = u[i + j] - (p & UINT_MAX) - k;
            u[i + j] = t;
            k = (p >> 32) - (t >> 32);
        }
        u[j + n] = t = u[j + n] - k;

        d[j] = q;
        if (t < 0) {
            assert(q > 0 && t == -1);
            d[j]--;
            k = 0;
            for (int i = 0; i < n; i++) {
                t = u[i + j] + k + v[i];
                u[i + j] = t;
                k = t > UINT_MAX;
            }
            u[j + n] = u[j + n] + k;
        }
        assert(u[j + n] == 0);
    }

    u.trim(), u >>= c, d.trim();
    swap(u, d);
    return d;
}

bigint div_mod(bigint& u, const bigint& v) {
    bigint r;
    if (magnitude_cmp(u, v)) {
        r = move(u);
        u.clear();
    } else if (v.len() == 1) {
        r = bigint(div_int(u, v[0]), u.sign);
        u.sign ^= v.sign, r.sign &= !r.zero();
    } else {
        r = div_vec(u, v);
    }
    return r;
}

} // namespace bigint_routines

inline namespace bigint_arithmetic {

bigint& operator+=(bigint& u, const bigint& v) {
    u.sign == v.sign ? add_vec(u, v) : dyn_sub_vec(u, v);
    return u;
}
bigint& operator-=(bigint& u, const bigint& v) {
    u.sign != v.sign ? add_vec(u, v) : dyn_sub_vec(u, v);
    return u;
}
bigint& operator*=(bigint& u, const bigint& v) {
    u = mul_vec(u, v);
    return u;
}
bigint& operator/=(bigint& u, const bigint& v) {
    div_mod(u, v);
    return u;
}
bigint& operator%=(bigint& u, const bigint& v) {
    u = div_mod(u, v);
    return u;
}

bigint& operator+=(bigint& u, uint n) {
    u.sign == 0 ? add_int(u, n) : sub_int(u, n);
    return u;
}
bigint& operator-=(bigint& u, uint n) {
    u.sign == 1 ? add_int(u, n) : sub_int(u, n);
    return u;
}
bigint& operator*=(bigint& u, uint n) {
    mul_int(u, n);
    return u;
}
bigint& operator/=(bigint& u, uint n) {
    div_int(u, n);
    return u;
}
bigint& operator%=(bigint& u, uint n) {
    u = bigint(div_int(u, n), u.sign);
    return u;
}

bigint& operator+=(bigint& u, int n) {
    n >= 0 ? u += uint(n) : u -= uint(abs(n));
    return u;
}
bigint& operator-=(bigint& u, int n) {
    n >= 0 ? u -= uint(n) : u += uint(abs(n));
    return u;
}
bigint& operator*=(bigint& u, int n) {
    mul_int(u, abs(n)), u.sign ^= n < 0;
    return u;
}
bigint& operator/=(bigint& u, int n) {
    div_int(u, abs(n)), u.sign ^= n < 0;
    return u;
}
bigint& operator%=(bigint& u, int n) {
    u = bigint(div_int(u, abs(n)), u.sign);
    return u;
}

bigint operator+(bigint u, const bigint& v) { return u += v; }
bigint operator+(bigint u, uint n) { return u += n; }
bigint operator+(bigint u, int n) { return u += n; }
bigint operator+(uint n, bigint u) { return u += n; }
bigint operator+(int n, bigint u) { return u += n; }

bigint operator-(bigint u, const bigint& v) { return u -= v; }
bigint operator-(bigint u, uint n) { return u -= n; }
bigint operator-(bigint u, int n) { return u -= n; }

bigint operator*(const bigint& u, const bigint& v) { return mul_vec(u, v); }
bigint operator*(bigint u, uint n) { return u *= n; }
bigint operator*(bigint u, int n) { return u *= n; }
bigint operator*(uint n, bigint u) { return u *= n; }
bigint operator*(int n, bigint u) { return u *= n; }

bigint operator/(bigint u, const bigint& v) { return u /= v; }
bigint operator/(bigint u, uint n) { return u /= n; }
bigint operator/(bigint u, int n) { return u /= n; }

bigint operator%(bigint u, const bigint& v) { return u %= v; }
bigint operator%(bigint u, uint n) { return u %= n; }
bigint operator%(bigint u, int n) { return u %= n; }

bigint operator-(bigint u) { return u.flip(), u; }
bool operator!(const bigint& u) { return u.zero(); }

bigint abs(bigint u) { return u.sign ? -u : u; }
bigint gcd(bigint a, bigint b) {
    while (a != 0) {
        b = b % a;
        swap(a, b);
    }
    return abs(b);
}

} // namespace bigint_arithmetic

bigint::bigint(const string& s, uint b) {
    assert(2 <= b && b <= 10);
    int i = 0, S = s.size();
    while (i < S && isspace(s[i])) {
        i++;
    }
    if (i == S) {
        return;
    }
    if (s[i] == '-') {
        sign = 1;
    }
    if (!('0' <= s[i] && s[i] <= '9')) {
        i++;
    }
    uint n = 0, tens = 1, threshold = UINT_MAX / (b + 1);
    while (i < S && ('0' <= s[i] && s[i] <= '9')) {
        n = b * n + uint(s[i++] - '0');
        tens *= b;
        if (tens >= threshold) {
            mul_int(*this, tens);
            add_int(*this, n);
            n = 0;
            tens = 1;
        }
    }
    mul_int(*this, tens);
    add_int(*this, n);
}

string to_string(bigint u, uint b = 10) {
    static auto uint_to_string = [](uint n, uint base) {
        string s;
        while (n > 0) {
            uint m = n / base;
            s += '0' + (n - base * m), n = m;
        }
        reverse(begin(s), end(s));
        return s;
    };

    if (u.zero())
        return "0";
    string s = u.sign ? "-" : "";
    vector<string> rems;
    uint divisor = b, digits = 1;
    while (divisor < UINT_MAX / b) {
        divisor *= b, digits++;
    }
    while (!u.zero()) {
        rems.push_back(uint_to_string(div_int(u, divisor), b));
    }
    for (int i = 0, n = rems.size(); i < n; i++) {
        string pad(i ? digits - rems[n - i - 1].length() : 0, '0');
        s += pad + rems[n - i - 1];
    }
    return s;
}

ostream& operator<<(ostream& out, const bigint& u) { return out << to_string(u); }

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

string to_string(const bfrac& f) { return to_string(f.n) + '/' + to_string(f.d); }

ostream& operator<<(ostream& out, const bfrac& f) { return out << to_string(f); }

bigint nCk(int n, int k) {
    if (k < 0 || k > n)
        return 0;
    k = min(k, n - k);
    n = n - k + 1;
    bigint binom = 1;
    int i = 1;
    while (i <= k) {
        binom = (binom * n++) / i++;
    }
    return binom;
}

} // namespace

#define MAXQ 120
int N, Q, A[3][MAXQ], S[3];

auto solve1() {
    string ans(Q, 'x');
    for (int i = 0; i < Q; i++) {
        ans[i] = (2 * S[0] >= Q) ^ !A[0][i] ? 'T' : 'F';
    }
    bfrac score = bigint(2 * S[0] >= Q ? S[0] : Q - S[0]);
    return ans + " " + to_string(score);
}

auto solve2() {
    array<int, 2> cnt = {};
    for (int i = 0; i < Q; i++)
        cnt[A[1][i]]++;

    array<bigint, 2> trues = {};
    bigint sols = 0;

    const auto [a0, a1] = cnt;

    for (int q0 = 0; q0 <= min(a0, S[0]); q0++) {
        int q1 = S[0] - q0;
        int p1 = (a0 - q0) + q1;
        if (q1 > a1 || p1 != S[1])
            continue;

        bigint combs = nCk(a0, q0) * nCk(a1, q1);
        trues[0] += q0 * combs;
        trues[1] += q1 * combs;
        sols += combs;
    }

    array<int, 2> best = {};
    array<bfrac, 2> best_score = {};

    for (int k = 0; k < 2; k++) {
        if (2 * trues[k] >= cnt[k] * sols) {
            best[k] = 1;
            best_score[k] = bfrac(trues[k], sols);
        } else {
            best[k] = 0;
            best_score[k] = bfrac(cnt[k]) - bfrac(trues[k], sols);
        }
    }

    bfrac score = best_score[0] + best_score[1];

    string ans(Q, 'x');
    for (int i = 0; i < Q; i++) {
        int k = A[1][i];
        ans[i] = best[k] ^ !A[0][i] ? 'T' : 'F';
    }
    return ans + " " + to_string(score);
}

auto solve3() {
    array<int, 4> cnt = {};
    for (int i = 0; i < Q; i++)
        cnt[A[1][i] + 2 * A[2][i]]++;

    array<bigint, 4> trues = {};
    bigint sols = 0;

    const auto [a0, a1, a2, a3] = cnt;

    for (int q0 = 0; q0 <= min(a0, S[0]); q0++) {
        for (int q1 = 0; q1 <= min(a1, S[0] - q0); q1++) {
            for (int q2 = 0; q2 <= min(a2, S[0] - q0 - q1); q2++) {
                int q3 = S[0] - q0 - q1 - q2;
                int p1 = (a0 - q0) + q1 + (a2 - q2) + q3;
                int p2 = (a0 - q0) + (a1 - q1) + q2 + q3;
                if (q3 > a3 || p1 != S[1] || p2 != S[2])
                    continue;

                bigint combs = nCk(a0, q0) * nCk(a1, q1) * nCk(a2, q2) * nCk(a3, q3);
                trues[0] += q0 * combs;
                trues[1] += q1 * combs;
                trues[2] += q2 * combs;
                trues[3] += q3 * combs;
                sols += combs;
            }
        }
    }

    array<int, 4> best = {};
    array<bfrac, 4> best_score = {};

    for (int k = 0; k < 4; k++) {
        if (2 * trues[k] >= cnt[k] * sols) {
            best[k] = 1;
            best_score[k] = bfrac(trues[k], sols);
        } else {
            best[k] = 0;
            best_score[k] = bfrac(cnt[k]) - bfrac(trues[k], sols);
        }
    }

    bfrac score = best_score[0] + best_score[1] + best_score[2] + best_score[3];

    string ans(Q, 'x');
    for (int i = 0; i < Q; i++) {
        int k = A[1][i] + 2 * A[2][i];
        ans[i] = best[k] ^ !A[0][i] ? 'T' : 'F';
    }
    return ans + " " + to_string(score);
}

auto solve() {
    cin >> N >> Q;
    for (int n = 0; n < N; n++) {
        string line;
        cin >> ws >> line >> S[n];
        for (int i = 0; i < Q; i++)
            A[n][i] = line[i] == 'T';
    }
    for (int i = 0; i < Q; i++) {
        if (!A[0][i]) {
            for (int n = 1; n < N; n++) {
                A[n][i] = !A[n][i]; // flip others
            }
        }
    }
    return N == 1 ? solve1() : N == 2 ? solve2() : solve3();
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
