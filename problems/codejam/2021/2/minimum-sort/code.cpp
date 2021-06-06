#include <bits/stdc++.h>

using namespace std;

// *****

int N;

int MIN_QUERY(int i, int j) {
    assert(1 <= i && i < j && j <= N);
    cout << "M " << i << ' ' << j << endl;
    int x;
    cin >> x;
    assert(1 <= x && x <= N);
    return x;
}

void SWAP_QUERY(int i, int j) {
    if (i > j) {
        swap(i, j);
    }
    assert(1 <= i && i < j && j <= N);
    cout << "S " << i << ' ' << j << endl;
    int x;
    cin >> x;
    assert(x == 1);
}

void DONE_QUERY() {
    cout << "D" << endl;
    int x;
    cin >> x;
    if (x != 1) {
        exit(1);
    }
}

void solve() {
    for (int i = 1; i < N; i++) {
        int x = MIN_QUERY(i, N);
        if (i != x) {
            SWAP_QUERY(i, x);
        }
    }
    DONE_QUERY();
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    unsigned T;
    cin >> T >> N;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
