#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int inf = INT_MAX;

auto solve() {
    int N;
    cin >> N;
    vector<int> price_A(N), ha(N), price_B(N), hb(N);
    for (int i = 0; i < N; i++) {
        cin >> price_A[i];
    }
    for (int i = 0; i < N; i++) {
        cin >> ha[i];
    }
    for (int i = 0; i < N; i++) {
        cin >> price_B[i];
    }
    for (int i = 0; i < N; i++) {
        cin >> hb[i];
    }

    unordered_map<int, multiset<pair<int, int>>> A, B;
    for (int i = 0; i < N; i++) {
        A[price_A[i]].emplace(ha[i], i + 1);
        B[price_B[i]].emplace(hb[i], i + 1);
    }

    sort(begin(price_A), end(price_A));
    sort(begin(price_B), end(price_B));
    price_A.push_back(inf);
    price_B.push_back(inf);

    vector<pair<int, int>> ans;

    int s = 0;
    while (s < N) {
        int pa = price_A[s];
        int pb = price_B[s];
        int e = s + 1;
        while (price_A[e] == pa && price_B[e] == pb) {
            e++;
        }

        int items = e - s;

        if (price_A[e] != pa) {
            assert(items == int(A[pa].size()));
            assert(items <= int(B[pb].size()));

            // vector<pair<int, int>> run_ans;

            for (auto ait = begin(A[pa]); ait != end(A[pa]); ++ait) {
                auto [a_height, a_id] = *ait;
                // find tallest from B
                auto bit = B[pb].lower_bound({a_height, 0});
                if (bit == begin(B[pb])) {
                    cout << "impossible" << endl;
                    return;
                }
                auto [b_height, b_id] = *--bit;
                ans.emplace_back(a_id, b_id);
                B[pb].erase(bit);
            }
            // ans.insert(end(ans), run_ans);
        } else if (price_B[e] != pb) {
            assert(items == int(B[pb].size()));
            assert(items <= int(A[pa].size()));

            // vector<pair<int, int>> run_ans;

            for (auto bit = begin(B[pb]); bit != end(B[pb]); ++bit) {
                auto [b_height, b_id] = *bit;
                // find shortest from A
                auto ait = A[pa].lower_bound({b_height + 1, 0});
                if (ait == end(A[pa])) {
                    cout << "impossible" << endl;
                    return;
                }
                auto [a_height, a_id] = *ait;
                ans.emplace_back(a_id, b_id);
                A[pa].erase(ait);
            }
            // ans.insert(end(ans), run_ans);
        }

        s = e;
    }

    assert(int(ans.size()) == N);
    for (int i = 0; i < N; i++) {
        cout << ans[i].first << " \n"[i + 1 == N];
    }
    for (int i = 0; i < N; i++) {
        cout << ans[i].second << " \n"[i + 1 == N];
    }
}

// *****

int main() {
    solve();
    return 0;
}
