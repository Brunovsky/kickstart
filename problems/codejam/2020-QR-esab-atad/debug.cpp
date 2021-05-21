#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int REVERSE_MASK = 1, COMPLEM_MASK = 2;

int B;
vector<int> S;

int reverse_ref, complem_ref;
int reverse_val, complem_val;

void echo(int n) {
    fprintf(stderr, "%2d/%02d C(%d -> %d) R(%d -> %d) | ", n, B, complem_ref,
            complem_val, reverse_ref, reverse_val);
    for (int i = 1; i <= B; ++i)
        cerr << S[i] << (i == n ? '.' : ' ');
    cerr << "\n\n";
}

void reverse() {
    for (int i = 1; 2 * i < B + 1; ++i)
        swap(S[i], S[B + 1 - i]);
}

void complem() {
    for (int i = 1; i <= B; ++i)
        S[i] = 1 - S[i];
}

bool answer_judge() {
    string s;
    for (int i = 1; i <= B; ++i)
        s += S[i] ? '1' : '0';
    cout << s << endl;
    cin >> s;
    return s == "Y";
}

// reads once
int read_one(int i) {
    int v;
    cout << i << endl;
    cin >> v;
    fprintf(stderr, "S[%02d] <- %d\n", i, v);
    return v;
}

// reads twice
void read_pair(int i) {
    if (i > B)
        i = 1;
    i = min(i, B + 1 - i);
    int j = B + 1 - i;

    int l = S[i] = read_one(i);
    int r = S[j] = read_one(j);

    if (l == r && !complem_ref) {
        complem_ref = i;
        complem_val = l;
    }
    if (l != r && !reverse_ref) {
        reverse_ref = i;
        reverse_val = l;
    }
}

// reads twice
void adjust() {
    fprintf(stderr, "***** ADJUSTMENT\n");
    if (complem_ref) {
        int val = read_one(complem_ref);
        fprintf(stderr, "C(%d -> %d) read: %d\n", complem_ref, complem_val,
                val);
        if (val != complem_val) {
            complem();
            reverse_val = 1 - reverse_val;
            complem_val = val;
            fprintf(stderr, "   complement\n  ");
            echo(complem_ref);
        }
    } else {
        read_one(1);
    }

    if (reverse_ref) {
        int val = read_one(reverse_ref);
        fprintf(stderr, "R(%d -> %d) read: %d\n", reverse_ref, reverse_val,
                val);
        if (val != reverse_val) {
            reverse();
            reverse_val = val;
            fprintf(stderr, "   reverse\n  ");
            echo(reverse_ref);
        }
    } else {
        read_one(1);
    }
    fprintf(stderr, "***** END ADJUSTMENT\n");
}

auto solve() {
    S.assign(B + 1, 0);
    reverse_ref = complem_ref = reverse_val = complem_val = 0;

    read_pair(1);
    read_pair(2);
    read_pair(3);
    read_pair(4);
    read_pair(5);
    echo(5);

    for (int i = 6; 2 * i <= B; i += 4) {
        adjust();
        read_pair(i + 0); // 6, 10, 14, 18, ...
        read_pair(i + 1); // 7, 11, 15, 19, ...
        read_pair(i + 2); // 8, 12, 16, 20, ...
        read_pair(i + 3); // 9, 13, 17, 21, ...
        echo(i + 3);
    }

    return answer_judge();
}

// *****

int main() {
    unsigned T;
    cin >> T >> B;

    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        if (!solution)
            break;
    }
    return 0;
}
