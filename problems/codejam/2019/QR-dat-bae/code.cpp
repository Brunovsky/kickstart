#include <bits/stdc++.h>

using namespace std;

// *****

// To prevent dealing with rounding, we pretend that N is a multiple of 16
// by padding the responses of the judge with the text in the queries. In other words,
// we simulate the missing computers ourselves.

const vector<int> ALL = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

#define MAXN 1024
int N, B, F;

string QUERY(string store) {
    cout << store.substr(0, N) << endl;
    string answer;
    cin >> answer;
    answer += store.substr(N);
    return answer;
}

void GUESS(string guess) {
    cout << guess << endl;
    int ans;
    cin >> ans;
    assert(ans == 1);
}

string qS, q8, q4, q2, q1;

void setup() {
    qS = "00000000000000001111111111111111";
    q8 = "00000000111111110000000011111111";
    q4 = "00001111000011110000111100001111";
    q2 = "00110011001100110011001100110011";
    q1 = "01010101010101010101010101010101";
    int len = 32;
    while (len < MAXN) {
        qS += qS, q8 += q8, q4 += q4, q2 += q2, q1 += q1;
        len *= 2;
    }
}

void solve() {
    cin >> N >> B >> F;
    assert(F >= 5 && B <= 15);
    int K = (N + 15) / 16;
    string ranges = QUERY(qS) + ((K & 1) ? '1' : '0'); // pad for .find()
    string a8 = QUERY(q8);
    string a4 = QUERY(q4);
    string a2 = QUERY(q2);
    string a1 = QUERY(q1);
    vector<int> broken;
    int x = 0;
    for (int i = 0; i < K; i++) {
        int y = ranges.find((i & 1) ? '0' : '1', x);
        vector<int> ok, notok;
        for (int j = x; j < y; j++) {
            int b0 = a1[j] - '0', b1 = a2[j] - '0';
            int b2 = a4[j] - '0', b3 = a8[j] - '0';
            int n = 8 * b3 + 4 * b2 + 2 * b1 + b0;
            ok.push_back(n);
        }
        x = y;
        set_difference(begin(ALL), end(ALL), begin(ok), end(ok), back_inserter(notok));
        for (int n : notok)
            broken.push_back(n + 16 * i);
    }
    assert(broken.size() == uint(B));
    string guess;
    for (int i = 0; i < B; i++) {
        guess.append(i > 0, ' ');
        guess += to_string(broken[i]);
    }
    GUESS(guess);
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    setup();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
