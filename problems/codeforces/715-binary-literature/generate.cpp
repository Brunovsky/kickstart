#include "../../../../code/testing.hpp"

// *****

int main() {
    int T = 300'000;
    putln(T);
    for (int i = 0; i < T; i++) {
        int N = randint(6, 15);
        putln(N);
        putln(generate_any_string(2 * N, '0', '1'));
        putln(generate_any_string(2 * N, '0', '1'));
        putln(generate_any_string(2 * N, '0', '1'));
    }
    return 0;
}
