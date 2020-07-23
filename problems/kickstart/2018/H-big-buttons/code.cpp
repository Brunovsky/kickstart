#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, P;
    cin >> N >> P >> ws;
    vector<string> B(P);

    for (int i = 0; i < P; i++) {
        getline(cin, B[i]);
    }

    vector<bool> rm(P, false);
    for (int i = 0; i < P; i++) {
        if (rm[i]) {
            continue;
        }
        for (int j = i + 1; j < P; j++) {
            if (rm[j]) {
                continue;
            }
            auto diff = mismatch(begin(B[i]), end(B[i]), begin(B[j]), end(B[j]));
            if (diff.second == end(B[j])) {
                rm[i] = true;
                break;
            } else if (diff.first == end(B[i])) {
                rm[j] = true;
            }
        }
    }

    int read = 0, write = 0;
    while (read < P) {
        if (!rm[read]) {
            B[write++] = B[read];
        }
        read++;
    }
    ulong count = 1UL << ulong(N);
    for (int i = 0; i < write; i++) {
        ulong exp = N - B[i].length();
        count -= 1UL << exp;
    }
    return count;
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
