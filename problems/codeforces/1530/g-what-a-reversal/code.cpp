#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
#define long int64_t
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        int N, k;
        cin >> N >> k;
        string a, b;
        cin >> a >> b;

        if (a == b) {
            cout << 0 << '\n';
            continue;
        }

        int ones = count(begin(a), end(a), '1');
        if (ones != count(begin(b), end(b), '1')) {
            cout << -1 << '\n';
            continue;
        }
        if (k == 0 || k > ones) {
            cout << -1 << '\n';
            continue;
        }

        vector<array<int, 2>> swaps;

        auto slide = [&](int l, int r, int d) {
            assert(0 <= l && l <= r && r < N);
            assert(count(begin(a) + l, begin(a) + r + 1, '1') == k);

            if (d > 0) {
                if (d >= N - r) {
                    return false;
                }
                for (int i = r + 1; i <= r + d; i++) {
                    if (a[i] != '0') {
                        return false;
                    }
                }

                swaps.push_back({l, r + d});
                if (l != r) {
                    swaps.push_back({l + d, r + d});
                }

                auto sub = a.substr(l, r - l + 1);
                for (int i = r + d; i >= l + d; i--) {
                    a[i] = a[i - d];
                }
                for (int i = l; i < l + d; i++) {
                    a[i] = '0';
                }
            } else if (d < 0) {
                d = -d;
                if (d > l) {
                    return false;
                }
                for (int i = l - 1; i >= l - d; i--) {
                    if (a[i] != '0') {
                        return false;
                    }
                }

                swaps.push_back({l - d, r});
                if (l != r) {
                    swaps.push_back({l - d, r - d});
                }

                for (int i = l - d; i <= r - d; i++) {
                    a[i] = a[i + d];
                }
                for (int i = r; i > r - d; i--) {
                    a[i] = '0';
                }
            }
            return true;
        };
        auto rev = [&](int l, int r) {
            assert(0 <= l && l <= r && r < N);
            assert(count(begin(a) + l, begin(a) + r + 1, '1') == k);
            if (l != r) {
                swaps.push_back({l, r});
                reverse(begin(a) + l, begin(a) + r + 1);
            }
        };

        // first push everything to the right ones-k+1 times
        int L, R;
        {
            int l = N - 1, r = N - 1, window = a[N - 1] == '1';
            while (window < k) {
                window += a[--l] == '1';
            }
            while (a[r] != '1') {
                r--;
            }
            for (int o = 0; o <= ones - k; o++) {
                int target = N - 1 - o;
                int d = target - r;
                bool ok = slide(l, r, d);
                assert(ok);

                l += d, r += d;
                if (o == ones - k) {
                    debug(a, b, l, r);
                    break;
                }

                l--, r--;
                while (a[r] != '1') {
                    r--;
                }
                while (a[l] != '1') {
                    l--;
                }
            }
            L = l, R = r;
        }

        vector<int> bones;
        for (int i = 0; i < N; i++) {
            if (b[i] == '1') {
                bones.push_back(i);
            }
        }

        // now push them to the left selectively

        auto attempt = [&]() {
            int l = 0, r = 0, window = a[0] == '1';
            while (window < k) {
                window += a[++r] == '1';
            }
            while (a[l] != '1') {
                l++;
            }

            bool busted = false;
            for (int o = 0; o <= ones - k; o++) {
                int target = bones[o];

                int d = target - l;
                bool ok = slide(l, r, d);

                if (!ok) {
                    busted = true;
                    break;
                }

                l += d, r += d;
                if (o == ones - k) {
                    debug(a, b, l, r);
                    break;
                }

                l++, r++;
                while (a[r] != '1') {
                    r++;
                }
                while (a[l] != '1') {
                    l++;
                }
            }

            if (busted) {
                return false;
            }

            if (a != b) {
                rev(l, r);
            }
            return a == b;
        };

        auto attempt_saved = [&]() {
            auto a_save = a;
            auto swaps_save = swaps;
            if (attempt()) {
                return true;
            } else {
                a = a_save;
                swaps = swaps_save;
                return false;
            }
        };

        if (attempt_saved()) {
            cout << swaps.size() << '\n';
            for (auto [x, y] : swaps) {
                cout << x + 1 << ' ' << y + 1 << '\n';
            }
            continue;
        }
        rev(L, R);
        if (attempt_saved()) {
            cout << swaps.size() << '\n';
            for (auto [x, y] : swaps) {
                cout << x + 1 << ' ' << y + 1 << '\n';
            }
            continue;
        }

        cout << -1 << '\n';
    }
    return 0;
}
