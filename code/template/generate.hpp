#include <bits/stdc++.h>

using namespace std;

// *****

using intd = uniform_int_distribution<int>;
using boold = bernoulli_distribution;

mt19937 mt(random_device{}());
intd distN(1, 100000);
intd distV(1, 500000);

#define T 100

int main() {
    printf("%d\n", T);
    for (int t = 1; t <= T; t++) {
        int n = distN(mt);
        printf("%d\n", n);
        for (int i = 0; i < n; i++) {
            int a = distV(mt);
            int b = distV(mt);
            int c = distV(mt);
            printf("%d %d %d\n", a, b, c);
        }
    }
    return 0;
}
