#include <bits/stdc++.h>

using namespace std;

// *****

int cnt[26];
int digits[10];

int& get(char c) { return cnt[c - 'A']; }

void take(int n, const string& s) {
    for (char c : s)
        get(c) -= digits[n];
}

auto solve() {
    string s;
    getline(cin, s);
    for (char c : s)
        get(c)++;

    memset(digits, 0, sizeof(digits));
    digits[0] = get('Z'), take(0, "ZERO");
    digits[2] = get('W'), take(2, "TWO");
    digits[4] = get('U'), take(4, "FOUR");
    digits[6] = get('X'), take(6, "SIX");
    digits[8] = get('G'), take(8, "EIGHT");

    digits[1] = get('O'), take(1, "ONE");
    digits[3] = get('H'), take(3, "THREE");
    digits[5] = get('F'), take(5, "FIVE");
    digits[7] = get('S'), take(7, "SEVEN");
    digits[9] = get('I'), take(9, "NINE");

    string phone;
    for (int i = 0; i < 10; i++)
        phone += string(digits[i], '0' + i);
    return phone;
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
