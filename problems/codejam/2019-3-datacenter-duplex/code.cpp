#include <bits/stdc++.h>

using namespace std;

// *****

struct disjoint_set {
    int N;
    int count;
    vector<int> next, size;

    disjoint_set(int N) : N(N), count(N), next(N), size(N, 1) {
        iota(begin(next), end(next), 0);
    }

    int find(int i) {
        while (i != next[i]) {
            i = next[i] = next[next[i]];
        }
        return i;
    }

    void join(int i, int j) {
        i = find(i);
        j = find(j);
        if (i != j) {
            if (size[i] < size[j]) {
                swap(i, j);
            }
            next[j] = i;
            size[i] += size[j];
            count--;
        }
    }
};

#define MAXS 101

int R, C;
char duplex[MAXS][MAXS];
char connection[MAXS][MAXS];

auto solve() {
    memset(duplex, 0, sizeof(duplex));
    memset(connection, '.', sizeof(connection));
    cin >> R >> C;
    for (int r = 0; r < R; r++) {
        string line;
        cin >> line;
        for (int c = 0; c < C; c++) {
            duplex[r][c] = line[c];
        }
    }
    disjoint_set dset(R * C);
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            char c1 = duplex[r + 0][c + 0], c2 = duplex[r + 0][c + 1];
            char c3 = duplex[r + 1][c + 0];
            int id1 = (r + 0) * C + (c + 0), id2 = (r + 0) * C + (c + 1);
            int id3 = (r + 1) * C + (c + 0);
            if (c1 == c2)
                dset.join(id1, id2);
            if (c1 == c3)
                dset.join(id1, id3);
        }
    }
    for (int r = 0; r + 1 < R; r++) {
        for (int c = 0; c + 1 < C; c++) {
            char c1 = duplex[r + 0][c + 0], c2 = duplex[r + 0][c + 1];
            char c3 = duplex[r + 1][c + 0], c4 = duplex[r + 1][c + 1];
            int id1 = (r + 0) * C + (c + 0), id2 = (r + 0) * C + (c + 1);
            int id3 = (r + 1) * C + (c + 0), id4 = (r + 1) * C + (c + 1);
            if (c1 == c4 && c2 == c3 && c1 != c2) {
                if (dset.find(id1) != dset.find(id4)) {
                    dset.join(id1, id4);
                    connection[r][c] = '\\';
                } else if (dset.find(id2) != dset.find(id3)) {
                    dset.join(id2, id3);
                    connection[r][c] = '/';
                }
            }
        }
    }
    return dset.count == 2 ? "POSSIBLE" : "IMPOSSIBLE";
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
        if (solution == "POSSIBLE"s) {
            for (int r = 0; r + 1 < R; r++) {
                for (int c = 0; c + 1 < C; c++) {
                    cout << connection[r][c];
                }
                cout << '\n';
            }
        }
    }
    return 0;
}
