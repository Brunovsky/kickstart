#include <bits/stdc++.h>

using namespace std;

// *****

using intd = uniform_int_distribution<int>;
using reald = uniform_real_distribution<double>;
using boold = bernoulli_distribution;

mt19937 mt(random_device{}());
intd distN(30, 100);
reald distV(0.0001, 10000.0);
reald distX(0.1, 99.9);
reald distR(0.0001, 10000.0);
reald distC(0.1, 99.9);

#define T 100

int main() {
    printf("%d\n", T);
    for (int t = 1; t <= T; t++) {
        int N = distN(mt);
        printf("%d %lf %lf\n", N, distV(mt), distX(mt));
        for (int i = 0; i < N; i++) {
            printf("%lf %lf\n", distR(mt), distC(mt));
        }
    }
    return 0;
}
