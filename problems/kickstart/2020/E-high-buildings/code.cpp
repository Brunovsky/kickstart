#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, A, B, C;
    cin >> N >> A >> B >> C;
    int onlyA = A - C;
    int onlyB = B - C;
    int M = N - onlyA - onlyB - C;
    // idiotic numbers
    if (C == 0 || C > A || C > B || onlyA + onlyB + C > N) {
        return "IMPOSSIBLE"s;
    }
    // no place to hide small buildings
    if (onlyA == 0 && onlyB == 0 && C == 1 && C < N) {
        return "IMPOSSIBLE"s;
    }
    if (N == 1) {
        return "1"s;
    }
    if (N == 2) {
        if (A == 1 && B == 2)
            return "2 1"s;
        else if (A == 2 && B == 1)
            return "1 2"s;
        else if (A == 2 && B == 2)
            return "1 1"s;
        assert(false);
    }
    vector<int> ans(N);
    // put buildings just for A and then just for B.
    for (int i = 0; i < onlyA; i++)
        ans[i] = 2;
    for (int i = 0; i < onlyB; i++)
        ans[N - 1 - i] = 2;
    // put hidden M buildings somewhere in the middle.
    int i = onlyA;
    if (C > 1) {
        // put in the middle of the C buildings
        for (int j = 0; j < C - 1; j++)
            ans[i + j] = 3;
        i += C - 1;
        for (int j = 0; j < M; j++)
            ans[i + j] = 1;
        i += M;
        ans[i] = 3;
    } else if (onlyA > 0) {
        // put between A and C
        for (int j = 0; j < M; j++)
            ans[i + j] = 1;
        i += M;
        for (int j = 0; j < C; j++)
            ans[i + j] = 3;
    } else if (onlyB > 0) {
        // put between B and C
        for (int j = 0; j < C; j++)
            ans[i + j] = 3;
        i += C;
        for (int j = 0; j < M; j++)
            ans[i + j] = 1;
    } else {
        assert(false);
    }

    string str;
    for (int j = 0; j < N; j++)
        str += (j ? " " : "") + to_string(ans[j]);
    return str;
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
