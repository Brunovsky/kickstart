#include <bits/stdc++.h>

using namespace std;

// *****

#define FOR_EACH_BIT(mask, n, bit)                                                      \
    for (uint z##n##bit = (mask), bit = z##n##bit & -z##n##bit, n = __builtin_ctz(bit); \
         z##n##bit; z##n##bit ^= bit,                                                   \
              z##n##bit && (bit = z##n##bit & -z##n##bit, (n = __builtin_ctz(bit))))

void solve() {
    int N, Q;
    cin >> N >> Q;
    vector<string> names(N);
    for (int i = 0; i < N; i++)
        cin >> names[i];

    vector<uint> mask(N, 0);
    for (int i = 0; i < N; i++)
        for (char c : names[i])
            mask[i] |= 1 << (c - 'A');

    vector<vector<uint>> jump(26, vector<uint>(26, 0)); // jump[char][distance]
    for (int c = 0; c < 26; c++)
        jump[c][0] = 1 << c;

    for (int d = 1; d < 26; d++) {
        for (int c = 0; c < 26; c++) {
            for (int i = 0; i < N; i++) {
                if (mask[i] & jump[c][d - 1]) {
                    jump[c][d] |= mask[i];
                }
            }
        }
    }

    for (int i = 0; i < Q; i++) {
        int x, y;
        cin >> x >> y, x--, y--;

        int dmin = 26;
        FOR_EACH_BIT (mask[x], c, n) {
            for (int d = 0; d < 26; d++) {
                if (jump[c][d] & mask[y]) {
                    dmin = min(d, dmin);
                    break;
                }
            }
        }
        cout << " " << (dmin < 26 ? dmin + 2 : -1);
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
        cout << '\n';
    }
    return 0;
}
