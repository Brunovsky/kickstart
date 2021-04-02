#include "../../../../code/debug_print.hpp"
#include "../../../../code/random.hpp"

// *****

intd distN(100000, 200000);
intd distV(1, 100);
intd distQ(10, 20);

#define T 10

int main() {
    print("{}\n", T);
    for (int t = 1; t <= T; t++) {
        int N = distN(mt);
        int Q = distQ(mt);
        print("{} {}\n", N, Q);
        for (int i = 0; i < N; i++) {
            print("{}{}", i ? " " : "", distV(mt));
        }
        print("\n");
        long M = 1L * N * (N + 1) / 2;
        longd distq(1, M);
        for (int q = 0; q < Q; q++) {
            long a = distq(mt), b = distq(mt);
            if (a > b)
                swap(a, b);
            print("{} {}\n", a, b);
        }
    }
    return 0;
}
