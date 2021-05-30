#include <bits/stdc++.h>

using namespace std;

#define long int64_t

// *****

auto solve() {
    int a, b, c;
    cin >> a >> b >> c;
    if (a == b)
        return c;
    else if (a == c)
        return b;
    else if (b == c)
        return a;
    else
        return 0;
}

// *****

int main() {
    ios::sync_with_stdio(false);
    cout << solve() << endl;
    return 0;
}
