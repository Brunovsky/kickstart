#include <bits/stdc++.h>

using namespace std;

int Q = 6;

int main() {
    int MAX_MASK = 1 << Q;
    string line1(Q, 'F'), line2(Q, 'F'), line3(Q, 'F');
    int T = MAX_MASK * MAX_MASK * MAX_MASK * (Q + 1) * (Q + 1);

    printf("%d\n", T);
    for (int A1 = 0; A1 < MAX_MASK; ++A1) {
        for (int A2 = 0; A2 < MAX_MASK; ++A2) {
            for (int B = 0; B < MAX_MASK; ++B) {
                for (int S1 = 0; S1 <= Q; ++S1) {
                    for (int S2 = 0; S2 <= Q; ++S2) {
                        for (int i = 0; i < Q; i++) {
                            line1[i] = (A1 & (1 << i)) ? 'T' : 'F';
                            line2[i] = (A2 & (1 << i)) ? 'T' : 'F';
                            line3[i] = (B & (1 << i)) ? 'T' : 'F';
                        }
                        printf("%d %d\n", 2, Q);
                        printf("%s\n", line1.c_str());
                        printf("%s\n", line2.c_str());
                        printf("%s\n", line3.c_str());
                        printf("%d %d\n", S1, S2);
                        T--;
                    }
                }
            }
        }
    }
    assert(T == 0);
    return 0;
}
