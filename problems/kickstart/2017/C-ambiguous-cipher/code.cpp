#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    string W;
    cin >> W >> ws;
    int w = W.length();

    if (w == 2) {
        swap(W[0], W[1]);
        return W;
    }

    int count = 0;
    string solution;
    for (int z = 0; z < 26; ++z) {
        string D(w, ' ');

        D[0] = 'A' + z;
        D[1] = W[0];
        D[w - 2] = W[w - 1];

        for (int i = 2; i < w; ++i) {
            int l = D[i - 2] - 'A';
            int m = W[i - 1] - 'A';
            if (D[i] != ' ') {
                int r = D[i] - 'A';
                if (m != ((l + r) % 26))
                    goto try_next;
            } else {
                int r = (m - l + 26) % 26;
                D[i] = r + 'A';
            }
        }

        ++count;
        if (count == 2) {
            return "AMBIGUOUS"s;
        }
        solution = D;
    try_next:
        continue;
    }

    return solution;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
