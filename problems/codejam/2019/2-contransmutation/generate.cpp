#include <bits/stdc++.h>

using namespace std;

// *****

using intd = uniform_int_distribution<long>;
using boold = bernoulli_distribution;

mt19937 mt(random_device{}());
intd dist(1, 1000000);
intd distM(2, 10);
boold boolF(0.10);
intd distF(1, 10);

#define T 100

int main() {
    printf("%d\n", T);
    for (int t = 1; t <= T; t++) {
        int M = distM(mt);
        intd distU(1, M);

        printf("\n%d\n", M);
        for (int i = 1; i <= M; i++) {
            int u = distU(mt);
            int v = distU(mt);
            while (u == v)
                v = distU(mt);

            printf("%d %d\n", u, v);
        }

        for (int i = 1; i <= M; i++) {
            long fuel = boolF(mt) ? distF(mt) : 0;
            printf("%s%ld", i > 1 ? " " : "", fuel);
        }
        printf("\n");
    }
    return 0;
}
