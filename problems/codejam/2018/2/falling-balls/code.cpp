#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXC 102

int C;
int B[MAXC];
int A[MAXC];

void solve() {
    memset(B, 0, sizeof(B));
    memset(A, 0, sizeof(A));
    cin >> C;
    for (int i = 0, j = 0; i < C; i++) {
        cin >> B[i];
        fill(A + j, A + j + B[i], i);
        j += B[i];
    }
    if (B[0] == 0 || B[C - 1] == 0) {
        cout << "IMPOSSIBLE\n";
        return;
    }
    string empty_line(C, '.');
    int L = 0;
    vector<string> ans;
    for (int i = 0; i < C; i++) {
        if (A[i] < i) {
            // ./
            // /.   go left
            L = max(L, i - A[i]);
            ans.resize(L, empty_line);
            for (int j = i; j > A[i]; j--) {
                assert(ans[i - j][j] == '.');
                ans[i - j][j] = '/';
            }
        } else if (A[i] > i) {
            // \.
            // .\   go right
            L = max(L, A[i] - i);
            ans.resize(L, empty_line);
            for (int j = i; j < A[i]; j++) {
                assert(ans[j - i][j] == '.');
                ans[j - i][j] = '\\';
            }
        }
    }
    ans.push_back(empty_line);

    cout << ans.size() << '\n';
    for (const string& line : ans) {
        cout << line << endl;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
    }
    return 0;
}
