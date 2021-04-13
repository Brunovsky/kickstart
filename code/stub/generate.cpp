#include "../../code/testing.hpp"

// *****

const int T = 10;

int main() {
    // ofstream other("correct.txt");
    putln(T);
    for (int t = 1; t <= T; t++) {
        int N = randint(1, 5);
        putln(N);
        for (int i = 0; i < N; i++) {
            int a = randint(1, 100'000);
            int b = randint(1, 100'000);
            int c = randint(1, 100'000);
            putln(a, b, c);
        }
    }
    return 0;
}
