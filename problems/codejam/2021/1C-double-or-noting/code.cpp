#include <bits/stdc++.h>

using namespace std;

// *****

vector<int> make_blocks(const string& s) {
    vector<int> c;
    for (int i = 0, S = s.size(); i < S; i++) {
        if (i == 0 || s[i - 1] != s[i]) {
            c.push_back(1);
        } else {
            c.back()++;
        }
    }
    return c;
}

#define even(a) !(a & 1)

auto solve() {
    string s, e;
    cin >> s >> e;
    int moves = 0;
    if (s == "0") {
        moves++, s = "1";
    }
    if (s == e) {
        return to_string(moves);
    }

    auto a = make_blocks(s);
    auto b = make_blocks(e);

    if (e == "0") {
        return to_string(a.size() - moves);
    }

    int A = a.size(), B = b.size();
    if ((A + 1) < B || (A < B && even(A))) {
        return "IMPOSSIBLE"s;
    }
    int ans = INT_MAX;

    if (A < B) {
        a.push_back(1), moves++, A++;
    }

    assert(A >= B);

    // we're gonna try every alignment possible
    for (int offset = A - B; offset <= A; offset++) {
        // align a[i + offset] with b[i].
        bool ok = true;
        for (int i = 0; ok && i + offset < A; i++) {
            if (i + offset + 1 == A && A > 1 && (A > B || even(A))) {
                ok = a[i + offset] <= b[i];
            } else {
                ok = a[i + offset] == b[i];
            }
        }
        if (!ok) {
            continue;
        }
        // Possible!
        int inserts = 0;
        if (A != offset) {
            inserts += b[A - 1 - offset] - a[A - 1];
        }
        for (int i = A - offset; i < B; i++) {
            inserts += b[i];
        }
        // printf("offset %d --> inserts = %d\n", offset, inserts);
        ans = min(ans, offset + inserts);
    }

    return to_string(ans + moves);
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
