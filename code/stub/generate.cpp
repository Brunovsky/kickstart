#include "../../../../code/debug_print.hpp"
#include "../../../../code/random.hpp"

// *****

template <typename T = int>
auto randint(T a, T b) {
    return uniform_int_distribution<T>(a, b)(mt);
}
template <typename T = double>
auto randreal(T a, T b) {
    return uniform_real_distribution<T>(a, b)(mt);
}

const int T = 100;

int main() {
    print("{}\n", T);
    for (int t = 1; t <= T; t++) {
        int N = randint(1, 500'000);
        print("{}\n", N);
        for (int i = 0; i < N; i++) {
            int a = randint(1, 100'000);
            int b = randint(1, 100'000);
            int c = randint(1, 100'000);
            print("{} {} {}\n", a, b, c);
        }
    }
    return 0;
}
