#include "../../code/hacking.hpp"

// *****

int T = 100;

int main(int argc, char** argv) {
    if (argc >= 2) // seed mt
        mt.seed(stoull(argv[1])), argc--, argv++;
    if (argc >= 2) // read batch size
        T = stoi(argv[1]), argc--, argv++;

    vector<long long> v;
    while (argc >= 2)
        v.push_back(stoll(argv[1])), argc--, argv++;

    ofstream ans("answer.txt");
    putln(T);
    for (int t = 1; t <= T; t++) {
        putln("::hack", t);
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
