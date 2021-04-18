#include "../../../code/hacking.hpp"

// *****

int T = 200'000;

int main(int argc, char** argv) {
    if (argc >= 2) // seed mt
        mt.seed(stoull(argv[1])), argc--, argv++;
    if (argc >= 2) // read batch size
        T = stoi(argv[1]), argc--, argv++;

    putln(T);
    for (int t = 1; t <= T; t++) {
        putln("::hack", t);
        putln(randint(1, 1'000'000'000), randint(1, 200'000));
    }
    return 0;
}
