#include <bits/stdc++.h>

using namespace std;

// *****

inline int min4(int a, int b, int c, int d) {
    return min(min(a, b), min(c, d));
}

auto solve() {
    int K;
    cin >> K;

    int n1 = 0, n2 = 0, n3 = 0, n4 = 0;

    int a;
    cin >> a;

    for (int i = 1; i < K; ++i) {
        int b;
        cin >> b;

        int m1, m2, m3, m4;

        if (a == b) {
            m1 = min4(n1, n2 + 1, n3 + 1, n4 + 1);
            m2 = min4(n1 + 1, n2, n3 + 1, n4 + 1);
            m3 = min4(n1 + 1, n2 + 1, n3, n4 + 1);
            m4 = min4(n1 + 1, n2 + 1, n3 + 1, n4);
        }
        if (a < b) {
            m1 = min4(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
            m2 = min4(n1, n2 + 1, n3 + 1, n4 + 1);
            m3 = min4(n1, n2, n3 + 1, n4 + 1);
            m4 = min4(n1, n2, n3, n4 + 1);
        }
        if (a > b) {
            m1 = min4(n1 + 1, n2, n3, n4);
            m2 = min4(n1 + 1, n2 + 1, n3, n4);
            m3 = min4(n1 + 1, n2 + 1, n3 + 1, n4);
            m4 = min4(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
        }

        a = b;
        n1 = m1;
        n2 = m2;
        n3 = m3;
        n4 = m4;
    }

    return min4(n1, n2, n3, n4);
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
