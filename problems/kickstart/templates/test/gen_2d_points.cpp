#include <random>
#include <iostream>

using namespace std;

#define SEPARATOR " "

random_device rng;
mt19937 mt(rng());
uniform_int_distribution<int> Xdist(-100000, 100000);
uniform_int_distribution<int> Ydist(-10000, 10000);

int main(int argc, char **argv) {
    int N = argc >= 2 ? atoi(argv[1]) : 300;
    printf("%d\n", N);
    for (int i = 0; i < N; i++) {
        int x = Xdist(mt);
        int y = Ydist(mt);
        printf("%d" SEPARATOR "%d\n", x, y);
    }
    return 0;
}
