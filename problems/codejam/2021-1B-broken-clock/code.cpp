#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long ns = 1'000'000'000;
constexpr long midday = ns * 60 * 60 * 12;

// Compute x, y such that ax + by = gcd(a,b)
long gcd(long a, long b, long& x, long& y) {
    long xn = 1, yn = 0;
    x = 0, y = 1;
    while (a != 0) {
        long q = b / a;
        b = b % a;
        x = x - q * xn, y = y - q * yn;
        swap(a, b), swap(x, xn), swap(y, yn);
    }
    if (b < 0) {
        b = -b, x = -x, y = -y;
    }
    return b;
}

bool run(long H, long M, long S) {
    long x, y;
    long g = gcd(11, 59, x, y);
    assert(g == 1 && 11 * x + 59 * y == 1);
    // chinese:
    // H = D + K (mod midday)
    // M = D + 12K (mod midday)
    // S = D + 720K (mod midday)
    // 11D = 12H - M (mod midday)
    // 59D = 60M - S (mod midday)
    // D = (11x + 59y)D = (12H - M)x + (60M - S)y (mod midday)
    long D = (((12 * H - M) * x + (60 * M - S) * y) % midday + midday) % midday;
    long K = (H - D + midday) % midday;
    if (H != (D + K) % midday)
        return false;
    if (M != (D + 12 * K) % midday)
        return false;
    if (S != (D + 720 * K) % midday)
        return false;
    long n = K % ns;
    K = (K - n) / ns;
    long s = K % 60;
    K = (K - s) / 60;
    long m = K % 60;
    K = (K - m) / 60;
    long h = K;
    assert(0 <= h && h < 12);
    assert(0 <= m && m < 60);
    assert(0 <= s && s < 60);
    assert(0 <= n && n < ns);
    cout << h << ' ' << m << ' ' << s << ' ' << n;
    return true;
}

auto solve() {
    long A, B, C;
    cin >> A >> B >> C;
    return run(A, B, C) || run(A, C, B) || //
           run(B, A, C) || run(B, C, A) || //
           run(C, A, B) || run(C, B, A);
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
        cout << endl;
    }
    return 0;
}
