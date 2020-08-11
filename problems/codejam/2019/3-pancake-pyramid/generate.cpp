#include <bits/stdc++.h>

using namespace std;

// *****

#define T 20
#define D 20

mt19937 mt(random_device{}());
uniform_int_distribution<int> distP(1, 1000000);
uniform_int_distribution<int> distS(3, 300);
uniform_int_distribution<int> distD(0, D - 1);

long P[D];
int S[T + 1];

int main() {
    for (int i = 0; i < D; i++)
        P[i] = distP(mt);
    // S[1] = 1'000'000;
    // S[2] = S[3] = S[4] = 100'000;
    for (int t = 1; t <= T; t++)
        S[t] = distS(mt);

    printf("%d\n", T);
    for (int t = 1; t <= T; t++) {
        printf("%d\n", S[t]);
        for (int i = 0; i < S[t]; i++) {
            printf("%ld ", P[distD(mt)]);
        }
        printf("\n");
    }
    return 0;
}
