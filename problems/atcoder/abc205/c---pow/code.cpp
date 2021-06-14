#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

string select(int a, int b) { return a < b ? "<" : a > b ? ">" : "="; }

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int A, B, C;
    cin >> A >> B >> C;
    if (C % 2 == 0) {
        A = abs(A);
        B = abs(B);
    }
    cout << select(A, B) << endl;

    return 0;
}
