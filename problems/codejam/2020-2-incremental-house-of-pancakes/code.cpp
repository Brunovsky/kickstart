#include <bits/stdc++.h>

using namespace std;

// *****

// use generate.cpp to generate inputs that will beat sqrt<ulong> precision, e.g:
// L=657108174072823537 R=143132403999732588

bool swapped;

string answer(ulong n, ulong L, ulong R) {
    if (swapped) {
        swap(L, R);
    }
    return to_string(n) + ' ' + to_string(L) + ' ' + to_string(R);
}

void fix_precision(ulong &v, ulong lower, ulong mid, ulong upper) {
    assert(lower < upper);
    if (mid < lower) {
        v--;
    } else if (upper <= mid) {
        v++;
    }
}

auto solve() {
    ulong L, R;
    cin >> L >> R >> ws;
    swapped = L < R;
    if (L < R) {
        swap(L, R);
    }
    ulong m = (sqrt(1 + 8 * (L - R)) - 1) / 2;
    fix_precision(m, m * (m + 1) / 2, L - R, (m + 1) * (m + 2) / 2);
    L -= m * (m + 1) / 2;
    swapped &= L != R;

    ulong n = (sqrt(m * m + 4 * L) - m) / 2;
    fix_precision(n, n * (m + n), L, (n + 1) * (m + n + 1));
    L -= n * (m + n);

    ulong k = (sqrt((m + 1) * (m + 1) + 4 * R) - (m + 1)) / 2;
    fix_precision(k, k * (m + k + 1), R, (k + 1) * (m + k + 2));
    R -= k * (m + k + 1);

    return answer(m + n + k, L, R);
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
