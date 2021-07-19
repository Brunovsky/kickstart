#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

void brute() {
    constexpr int D = 4;
    constexpr int MAX = 10000;

    vector<vector<int>> each = {{1, 2, 3}};
    for (int len = 1; len < D; len++) {
        each.push_back({});
        for (int n : each[len - 1]) {
            each[len].push_back(10 * n + 1);
            each[len].push_back(10 * n + 2);
            each[len].push_back(10 * n + 3);
        }
    }

    vector<int> nums;
    for (vector<int> block : each) {
        nums.insert(end(nums), begin(block), end(block));
    }
    debug(nums);
    int N = nums.size();

    debug(N * N * N * N);

    vector<int> rep(10 * MAX, 0);
    for (int n : nums) {
        for (int m : nums) {
            for (int k : nums) {
                for (int l : nums) {
                    rep[n + m + k + l] = 4;
                }
            }
        }
    }
    for (int n : nums) {
        for (int m : nums) {
            for (int k : nums) {
                rep[n + m + k] = 3;
            }
        }
    }
    for (int n : nums) {
        for (int m : nums) {
            rep[n + m] = 2;
        }
    }
    for (int n : nums) {
        rep[n] = 1;
    }

    rep.resize(MAX);

    for (int n = 1; n < MAX; n++) {
        if (rep[n] == 0) {
            cout << "BAD: " << n << '\n';
        }
    }
}

// don't take, don't send
// don't take, send
// take, don't send
// take, send
array<int, 4> patterns[10] = {{-1, 4, 3}, {3, 0}};

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        long number;
        cin >> number;
        string s = to_string(number);
        int N = s.size();

        if (all_of(begin(s), end(s), [&](char c) { return '1' <= c && c <= '3'; })) {
            cout << 1 << '\n';
            continue;
        }

        vector<int> digits(N + 1);
        for (int i = 0; i < N; i++) {
            digits[i + 1] = s[N - i - 1] - '0';
        }
        debug(digits);

        // let's try with K numbers
        // consider just digits[i]
        // at any point we need to know how many of the numbers can have another digit
        // added to them (dp on this number?)
        // we also need to know if we can send 1 carry to the next digit

        for (int K = 2; K <= 9; K++) {
            vector<vector<int8_t>> nocarry(N + 1, vector<int8_t>(K + 1));
            vector<vector<int8_t>> carry(N + 1, vector<int8_t>(K + 1));

            nocarry[0][K] = true;

            for (int i = 1; i <= N; i++) {
                int DIGIT = digits[i];
                for (int k = 1; k <= K; k++) {
                    if (nocarry[i - 1][k]) {
                        for (int g = 0; g <= k; g++) {
                            // can we reach g digits for i+1 with no carry?
                            // do we overflow with only 1s?
                            nocarry[i][g] |= g <= DIGIT && 3 * g >= DIGIT;

                            // can we reach g digits for i+1 with carry?
                            // do we underflow with only 3s? need DIGIT+10
                            carry[i][g] |= 3 * g >= DIGIT + 10;
                        }
                    }
                    if (carry[i - 1][k]) {
                        for (int g = 0; g <= k; g++) {
                            // can we reach g digits for i+1 with no carry?
                            // do we overflow with only 1s?
                            nocarry[i][g] |= g + 1 <= DIGIT && 3 * g + 1 >= DIGIT;

                            // can we reach g digits for i+1 with carry?
                            // do we underflow with only 3s? need DIGIT+10
                            carry[i][g] |= 3 * g + 1 >= DIGIT + 10;
                        }
                    }
                }
            }

#ifdef LOCAL
            debug(K, number);
            cerr << mat_to_string(nocarry) << '\n';
            cerr << mat_to_string(carry) << '\n';
#endif

            bool reached = false;
            for (int k = 0; k <= K; k++) {
                reached |= nocarry[N][k];
            }
            if (reached) {
                cout << K << '\n';
                break;
            }
        }
    }
    return 0;
}
