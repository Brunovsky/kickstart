#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N;
    cin >> N;
    int yen = 0;
    int day = 0;
    while (yen < N) {
        yen += ++day;
    }
    cout << day << '\n';
    return 0;
}
