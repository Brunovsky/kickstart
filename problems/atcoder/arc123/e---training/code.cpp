#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

uint64_t floor_sum_unsigned(uint64_t n, uint64_t m, uint64_t a, uint64_t b) {
    assert(n < (1LL << 32) && 1 <= m && m < (1LL << 32));
    uint64_t ans = 0;
    while (true) {
        if (a >= m) {
            ans += n * (n - 1) / 2 * (a / m);
            a %= m;
        }
        if (b >= m) {
            ans += n * (b / m);
            b %= m;
        }

        uint64_t y_max = a * n + b;
        if (y_max < m) {
            break;
        }

        n = y_max / m;
        b = y_max % m;
        swap(a, m);
    }
    return ans;
}

int64_t floor_sum(uint64_t n, uint64_t m, int64_t a, int64_t b) {
    assert(n < (1LL << 32) && 1 <= m && m < (1LL << 32));
    uint64_t ans = 0;
    if (a < 0) {
        int64_t a2 = a % m;
        a2 = a2 < 0 ? a2 + m : a;
        ans -= 1ULL * n * (n - 1) / 2 * ((a2 - a) / m);
        a = a2;
    }
    if (b < 0) {
        int64_t b2 = b % m;
        b2 = b2 < 0 ? b2 + m : b;
        ans -= 1ULL * n * ((b2 - b) / m);
        b = b2;
    }
    return ans + floor_sum_unsigned(n, m, a, b);
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        long N, Ax, Bx, Ay, By;
        cin >> N >> Ax >> Bx >> Ay >> By;
        if (Bx == By) {
            if (Ax == Ay) {
                cout << N << '\n';
            } else {
                cout << 0 << '\n';
            }
            continue;
        }
        if (Bx > By) {
            swap(Ax, Ay);
            swap(Bx, By);
        }
        assert(Bx < By);
        if (Ax > Ay) {
            cout << 0 << '\n';
            continue;
        }

        const long CEIL = By - Bx - 1, FLOOR = 0;
        long L = (Bx * By * (Ay - Ax - 1) + FLOOR) / (By - Bx);
        long M = (Bx * By * (Ay - Ax) + FLOOR) / (By - Bx);
        long R = (Bx * By * (Ay - Ax + 1) + CEIL) / (By - Bx) - 1;

        assert(L <= M && M <= R);
        L = clamp(L, 0L, N);
        M = clamp(M, 0L, N);
        R = clamp(R, 0L, N);
        assert(0 <= L && L <= M && M <= R && R <= N);

        auto lx = floor_sum(L + 1, Bx, 1, Ax * Bx);
        auto mx = floor_sum(M + 1, Bx, 1, Ax * Bx);
        auto rx = floor_sum(R + 1, Bx, 1, Ax * Bx);

        auto ly = floor_sum(L + 1, By, 1, Ay * By);
        auto my = floor_sum(M + 1, By, 1, Ay * By);
        auto ry = floor_sum(R + 1, By, 1, Ay * By);

        auto upper = (R - M) - ((rx - mx) - (ry - my));
        auto lower = (M - L) - ((my - ly) - (mx - lx));
        auto ans = lower + upper;

        cout << ans << '\n';
    }
    return 0;
}
