#include <bits/stdc++.h>

using namespace std;

// *****

mt19937 mt(random_device{}());
uniform_int_distribution<int> dist(1, 999'999'999);

#define T 1
#define U 9

string digits = "BCDEFGHIJA";

int main() {
    printf("%d\n", T);
    printf("%d\n", U);
    for (int t = 1; t <= T; t++) {
        for (int i = 0; i < 10'000; i++) {
            int Q = dist(mt);
            uniform_int_distribution<int> Mdist(1, Q);
            int M = Mdist(mt);
            string text;
            while (M > 0) {
                text += digits[M % 10];
                M /= 10;
            }
            reverse(begin(text), end(text));
            printf("%d %s\n", Q, text.data());
        }
    }
    return 0;
}
