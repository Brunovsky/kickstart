#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

void print(int a, int b) { cout << a << ' ' << b << '\n'; }

// Move n discs for s to t
void dfs_hanoi(int n, int s, int t) {
    if (n == 1) {
        print(s, t);
    } else {
        dfs_hanoi(n - 1, s, s ^ t);
        print(s, t);
        dfs_hanoi(n - 1, s ^ t, t);
    }
}

int main() {
    ios::sync_with_stdio(false);
    int n;
    cin >> n;
    cout << ((1 << n) - 1) << '\n';
    dfs_hanoi(n, 1, 3);
    return 0;
}
