#include "../../../code/hacking.hpp"

// *****

int T = 10000;

int main(int argc, char** argv) {
    int a = 1, b = 20;

    if (argc >= 2) // seed mt
        a = stoi(argv[1]), argc--, argv++;
    if (argc >= 2) // read batch size
        b = stoi(argv[1]), argc--, argv++;

    putln(T);
    for (int t = 1; t <= T; t++) {
        int N = randint(a, b);
        vector<int> nums;
        for (int i = 0; i < N; i++) {
            nums.push_back(N - i);
        }
        putln(N);
        putln(nums);
    }
    return 0;
}
