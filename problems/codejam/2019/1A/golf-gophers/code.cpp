#include <bits/stdc++.h>

using namespace std;

// *****

#define PRIMES 7
constexpr long primes[] = {4, 3, 5, 7, 11, 13, 17};
const long MOD = accumulate(primes, primes + PRIMES, 1, std::multiplies<long>{});

#define HOLES     18
#define MINBLADES 2
#define MAXBLADES 18

long N, M;

long gcd(long a, long b, long &x, long &y) {
    long xn = 1, yn = 0;
    x = 0, y = 1;
    while (a != 0) {
        long q = b / a;
        b = b % a;
        x = x - q * xn;
        y = y - q * yn;
        swap(a, b);
        swap(x, xn);
        swap(y, yn);
    }
    if (b < 0) {
        b = -b, x = -x, y = -y;
    }
    return b;
}

long query(long prime) {
    assert(MINBLADES <= prime && prime <= MAXBLADES);
    string query;
    for (long i = 0; i < HOLES; i++) {
        query.append(i > 0, ' ');
        query += to_string(prime);
    }
    cout << query << endl;
    long sum = 0;
    for (int i = 0; i < HOLES; i++) {
        long n;
        cin >> n;
        assert(n >= 0 && n < prime);
        sum += n;
    }
    return sum % prime;
}

long chinese(long remainders[]) {
    long p = 1, a = 0;
    for (int i = 0; i < PRIMES; i++) {
        long q = primes[i], b = remainders[i];
        long x, y;
        gcd(p, q, x, y);
        a = a * q * y + b * p * x;
        p = p * q;
        a = ((a % p) + p) % p;
    }
    return a;
}

void solve() {
    long remainders[PRIMES];
    for (int i = 0; i < PRIMES; i++) {
        remainders[i] = query(primes[i]);
    }
    long ans = chinese(remainders);
    cout << ans << endl;
    int verdict;
    cin >> verdict;
    assert(verdict == 1);
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    unsigned T;
    cin >> T >> N >> M;
    assert(N >= PRIMES && M < MOD);
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
