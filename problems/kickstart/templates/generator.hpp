#include <bits/stdc++.h>

using namespace std;

// *****

mt19937 mt(random_device{}());
uniform_int_distribution<int> dist(1, 1000000);

#define T 100

int main() {
    printf("%d\n", T);
    for (int t = 1; t <= T; t++) {
        int a = dist(mt);
        int b = dist(mt);
        int c = dist(mt);
        printf("%d %d %d\n", a, b, c);
    }
    return 0;
}
