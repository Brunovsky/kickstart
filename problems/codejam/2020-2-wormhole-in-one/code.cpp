#include <bits/stdc++.h>

using namespace std;

// *****

struct ratio_t {
    long num, den;
};

long gcd(long a, long b) {
    while (a != 0) {
        b = b % a;
        swap(a, b);
    }
    return abs(b);
}

bool operator==(ratio_t lhs, ratio_t rhs) {
    return lhs.num == rhs.num && lhs.den == rhs.den;
}

struct hasher {
    size_t operator()(ratio_t r) const noexcept {
        if (r.num == LONG_MAX) {
            return ULONG_MAX;
        } else {
            return 0x9e3779b9 + ((r.num >> 2) ^ (r.den << 6));
        }
    }
};

ratio_t ratio_of(long n, long d) {
    long g = gcd(n, d);
    n /= g, d /= g;
    if (d < 0) {
        n = -n, d = -d;
    }
    return {n, d};
}

ratio_t slope_of(long x1, long y1, long x2, long y2) {
    return x1 == x2 ? ratio_t{LONG_MAX, 0} : ratio_of(y1 - y2, x1 - x2);
}

ratio_t axis_of(long x1, long y1, long x2, long y2) {
    return x1 == x2 ? ratio_t{x1, 0} : ratio_of(x1 * y2 - x2 * y1, x1 - x2);
}

#define MAXN 101

int N;
int X[MAXN];
int Y[MAXN];
unordered_map<ratio_t, unordered_map<ratio_t, int, hasher>, hasher> lines;

auto solve() {
    lines.clear();
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> X[i] >> Y[i];
    }
    for (int i = 0; i < N; i++) {
        int x1 = X[i], y1 = Y[i];
        for (int j = i + 1; j < N; j++) {
            int x2 = X[j], y2 = Y[j];
            lines[slope_of(x1, y1, x2, y2)][axis_of(x1, y1, x2, y2)]++;
        }
    }
    int ans = 1;
    for (const auto& slope_entry : lines) {
        const auto& axis_set = slope_entry.second;
        int loops = 0;
        for (auto entry : axis_set) {
            int combinations = entry.second;
            loops += ceil(sqrt(2 * combinations));
            // (n choose 2) = cnt ==> n = ceil(sqrt(2 cnt))
        }
        loops -= loops & 1;
        int unaligned = N - loops;
        ans = max(ans, loops + min(unaligned, 2));
    }
    return ans;
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
