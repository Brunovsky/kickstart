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

    // Create a graph with the following nodes:
    // [start] for words beginning with 'start'
    // [end] for words ending with 'end'
    // Connect [end]->[start] if there is a word with both start and end.
    // Reachable leaf states are losing

    constexpr int A = 52;
    auto mask = [&](char c) {
        if ('a' <= c && c <= 'z') {
            return 0 + (c - 'a');
        } else {
            return 26 + (c - 'A');
        }
    };
    auto nodeid = [&](const string& s) {
        assert(s.size() == 3u);
        int a = mask(s[0]), b = mask(s[1]), c = mask(s[2]);
        assert(0 <= a && a < A);
        assert(0 <= b && b < A);
        assert(0 <= c && c < A);
        return A * A * a + A * b + c;
    };

    constexpr int W = A * A * A;

    int N;
    cin >> N;
    vector<string> words(N);
    for (int i = 0; i < N; i++) {
        cin >> words[i];
    }

    vector<vector<int>> out(W), in(W);
    vector<int> outdeg(W), indeg(W);
    for (int i = 0; i < N; i++) {
        int len = words[i].size();
        int start = nodeid(words[i].substr(0, 3));
        int end = nodeid(words[i].substr(len - 3));
        out[start].push_back(end);
        in[end].push_back(start);
    }
    for (int w = 0; w < W; w++) {
        outdeg[w] = out[w].size();
        indeg[w] = in[w].size();
    }

    vector<bool> reached(W);
    vector<bool> winning(W);
    vector<int> bfs(W);
    int S = 0;
    for (int u = 0; u < W; u++) {
        if (outdeg[u] == 0) {
            bfs[S++] = u;
            winning[u] = false;
            reached[u] = true;
        }
    }

    for (int i = 0; i < S; i++) {
        int v = bfs[i];
        if (!winning[v]) {
            for (int u : in[v]) {
                if (outdeg[u] > 0) {
                    outdeg[u] = 0;
                    bfs[S++] = u;
                    winning[u] = true;
                    reached[u] = true;
                }
            }
        } else {
            for (int u : in[v]) {
                if (--outdeg[u] == 0) {
                    bfs[S++] = u;
                    reached[u] = true;
                    winning[u] = false;
                }
            }
        }
    }

    for (int i = 0; i < N; i++) {
        int len = words[i].size();
        int end = nodeid(words[i].substr(len - 3));
        // Aoki receives end
        if (!reached[end]) {
            cout << "Draw\n";
        } else if (winning[end]) {
            cout << "Aoki\n";
        } else {
            cout << "Takahashi\n";
        }
    }

    return 0;
}
