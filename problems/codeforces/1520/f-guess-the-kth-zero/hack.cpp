#include "../../../code/hacking.hpp"

using namespace std;

struct fenwick {
    int N;
    vector<int> tree;

    explicit fenwick(int N = 0) : N(N), tree(N + 1, 0) {}

    int sum(int i) {
        int sum = 0;
        while (i > 0) {
            sum += tree[i];
            i -= i & -i;
        }
        return sum;
    }

    void add(int i, int n) {
        assert(i > 0);
        while (i <= N) {
            tree[i] += n;
            i += i & -i;
        }
    }

    int lower_bound(int n) {
        int i = 0;
        int bits = CHAR_BIT * sizeof(int) - __builtin_clz(N << 1);
        for (int j = 1 << bits; j; j >>= 1) {
            if (i + j <= N && tree[i + j] < n) {
                n -= tree[i + j];
                i += j;
            }
        }
        return i + 1;
    }
};

int main() {
    int N = 200000, Q = 10000;
    putln(N, Q);

    int total_zeros = 0;
    vector<int> A(N + 1);
    fenwick tree(N);
    for (int i = 1; i <= N; i++) {
        A[i] = randint(0, 1);
        tree.add(i, A[i]);
        total_zeros += A[i] == 0;
    }
    assert(total_zeros >= Q);
    for (int q = 0; q < Q; q++) {
        int how_many = randint(1, total_zeros);
        putln(how_many);
        while (true) {
            string command;
            cin >> command;
            if (command == "?") {
                int L, R;
                cin >> L >> R;
                // print(stderr, "Queried {} {}\n", L, R);
                int ones = tree.sum(R) - tree.sum(L - 1);
                putln(ones);
            } else {
                assert(command == "!");
                int i;
                cin >> i;
                int ones = tree.sum(i);
                int zeros = i - ones;
                if (zeros != how_many) {
                    print(stderr, "Incorrect guess {}, got {}, expected {}\n", i, zeros,
                          how_many);
                    exit(0);
                } else if (A[i] != 0) {
                    print(stderr, "Guess {} is not 0\n", i);
                    exit(0);
                } else if (q % 100 == 0) {
                    print(stderr, "Query {} OK: answer is {}\n", q, i);
                }
                A[i] = 1;
                tree.add(i, 1);
                break;
            }
        }
        total_zeros--;
    }
    return 0;
}
