#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N;
    cin >> N;
    int Y = 1.08 * N;
    if (Y < 206) {
        cout << "Yay!\n";
    } else if (Y == 206) {
        cout << "so-so\n";
    } else {
        cout << ":(\n";
    }
    return 0;
}
