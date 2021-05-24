#include "../../../code/hacking.hpp"

int N = 100;

int main(int argc, char** argv) {
    // if (argc >= 2) mt.seed(stoull(argv[1])), argc--, argv++; // seed mt
    if (argc >= 2)
        N = stoi(argv[1]), argc--, argv++; // read a batch size
    // while (argc >= 2) input.push_back(stoll(argv[1])), argc--, argv++; // read numbers

    // ofstream ans("answer.txt");
    // putln("::hack", t);
    int M = N * N;
    putln(N, M);
    for (int i = 0; i < N; i++) {
        int a = i;
        int b = randint(0, N - 1);
        int c = randint(1, 100'000'000);
        putln(a, b, c);
    }
    for (int i = N; i < M; i++) {
        int a = randint(0, N - 1);
        int b = randint(0, N - 1);
        int c = randint(1, 100'000'000);
        putln(a, b, c);
    }
    return 0;
}
