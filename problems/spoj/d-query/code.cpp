#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N;
    cin >> N;
    vector<int> a(N);
    for (int i = 0; i < N; i++) {
        cin >> a[i];
    }
    const int BLOCK = sqrt(N);
    const int NUM_BLOCKS = (N + BLOCK - 1) / BLOCK;
    const auto block = [&](int x) { return x / BLOCK; };
    int Q;
    cin >> Q;
    vector<array<int, 2>> queries(Q);
    vector<int> small_queries;
    vector<vector<int>> by_block(NUM_BLOCKS);
    for (int i = 0; i < Q; i++) {
        auto& [l, r] = queries[i];
        cin >> l >> r, l--, r--;
        if (block(l) == block(r)) {
            small_queries.push_back(i);
        } else {
            by_block[block(l)].push_back(i);
        }
    }
    vector<int> answer(Q);
    vector<int> freq(*max_element(begin(a), end(a)) + 1);
    int count_distinct = 0;
    const auto add = [&](int n) { count_distinct += freq[n]++ == 0; };
    const auto rem = [&](int n) { count_distinct -= --freq[n] == 0; };
    for (int q : small_queries) {
        auto [l, r] = queries[q];
        for (int i = l; i <= r; i++) {
            add(a[i]);
        }
        answer[q] = count_distinct;
        for (int i = l; i <= r; i++) {
            rem(a[i]);
        }
    }
    for (int b = 0; b < NUM_BLOCKS; b++) {
        sort(begin(by_block[b]), end(by_block[b]), [&](int x, int y) {
            return make_pair(queries[x][1], queries[x][0]) <
                   make_pair(queries[y][1], queries[y][0]);
        });
        int current_l = -1, current_r = -1;
        for (int q : by_block[b]) {
            auto [l, r] = queries[q];
            if (current_l == -1) {
                for (int i = l; i <= r; i++) {
                    add(a[i]);
                }
            } else {
                for (int i = current_r + 1; i <= r; i++) {
                    add(a[i]);
                }
                for (int i = l; i < current_l; i++) {
                    add(a[i]);
                }
                for (int i = current_l; i < l; i++) {
                    rem(a[i]);
                }
            }
            answer[q] = count_distinct;
            current_l = l, current_r = r;
        }
        for (int i = current_l; current_l != -1 && i <= current_r; i++) {
            rem(a[i]);
        }
        assert(count_distinct == 0);
    }
    for (int q = 0; q < Q; q++) {
        cout << answer[q] << '\n';
    }
    return 0;
}
