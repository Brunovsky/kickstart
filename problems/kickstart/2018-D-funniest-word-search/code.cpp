#include <bits/stdc++.h>

using namespace std;

// *****

// Knuth Morris Pratt (from library)

namespace {

class KMP {
    vector<int> table;
    string needle;

  public:
    KMP(string pattern) : needle(move(pattern)) {
        int P = needle.size();
        table.resize(P + 1);
        table[0] = -1;

        int b = 0;
        for (int j = 1; j < P; ++j, ++b) {
            if (needle[j] == needle[b]) {
                table[j] = table[b];
            } else {
                table[j] = b;
                do {
                    b = table[b];
                } while (b >= 0 && needle[j] != needle[b]);
            }
        }
        table[P] = b;
    }

    int lookup(int j) const { return table[j]; }
    int shift(int j) const { return j - table[j]; }
    const string& get_pattern() const { return needle; }
};

vector<int> kmp_search_all(const string& text, const KMP& kmp) {
    const string& needle = kmp.get_pattern();
    int P = needle.size(), T = text.size();
    int i = 0, j = 0;
    vector<int> match;

    while (i <= T - P) {
        while (j < P && text[i + j] == needle[j]) {
            j++;
        }
        if (j == P) {
            match.push_back(i);
        }
        i += kmp.shift(j);
        j = max(0, kmp.lookup(j));
    }

    return match;
}

} // namespace

#define MAXL 102
#define MAXW 1000

int R, C;
string text_rows[MAXL];
string text_cols[MAXL];
vector<string> words;
long H[MAXL][MAXL][MAXL], V[MAXL][MAXL][MAXL];

void fill_word_counts(const string& word) {
    if (word.empty())
        return;

    KMP kmp(word);
    const int l = word.size();

    for (int r = 1; r <= R; ++r) {
        auto indices = kmp_search_all(text_rows[r - 1], kmp);
        for (int c : indices) {
            H[r][c + l][l] += long(l);
        }
    }

    for (int c = 1; c <= C; ++c) {
        auto indices = kmp_search_all(text_cols[c - 1], kmp);
        for (int r : indices) {
            V[r + l][c][l] += long(l);
        }
    }
}

long gcd(long a, long b) {
    while (a != 0) {
        b = b % a;
        swap(a, b);
    }
    return abs(b);
}

auto solve() {
    int W;
    cin >> R >> C >> W >> ws;
    words.clear();
    for (int r = 0; r < R; ++r) {
        string text;
        cin >> text >> ws;
        assert(text.size() == uint(C));
        text_rows[r] = move(text);
    }
    for (int c = 0; c < C; ++c) {
        text_cols[c].resize(R);
        for (int r = 0; r < R; ++r) {
            text_cols[c][r] = text_rows[r][c];
        }
    }
    for (int w = 0; w < W; ++w) {
        string word;
        cin >> word;
        int len = word.size();
        if (len > R && len > C) {
            continue;
        }
        words.push_back(move(word));
    }

    memset(H, 0, sizeof(H));
    memset(V, 0, sizeof(V));

    for (string word : words) {
        fill_word_counts(word);
        reverse(word.begin(), word.end());
        fill_word_counts(word);
    }

    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            for (int l = 2; l <= C; ++l) {
                H[r][c][l] += H[r][c][l - 1]; // prefix sum lengths
            }
            for (int l = 2; l <= R; ++l) {
                V[r][c][l] += V[r][c][l - 1];
            }
        }
    }

    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            for (int l = 2; l <= C; ++l) {
                H[r][c][l] += H[r][c - 1][l - 1]; // [...|] --> [... ]
            }
            for (int l = 2; l <= R; ++l) {
                V[r][c][l] += V[r - 1][c][l - 1];
            }
        }
    }

    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            for (int l = 1; l <= C; ++l) {
                H[r][c][l] += H[r - 1][c][l]; // prefix prefix sum
            }
            for (int l = 1; l <= R; ++l) {
                V[r][c][l] += V[r][c - 1][l];
            }
        }
    }

    long best_len = 0, best_sum = R + C;
    long count = 0;
    // best == best_len / best_sum

    for (int ra = 0; ra < R; ++ra) {
        for (int rb = ra + 1; rb <= R; ++rb) {
            for (int ca = 0; ca < C; ++ca) {
                for (int cb = ca + 1; cb <= C; ++cb) {
                    long lh = cb - ca;
                    long lv = rb - ra;
                    long h = H[rb][cb][lh] - H[ra][cb][lh];
                    long v = V[rb][cb][lv] - V[rb][ca][lv];
                    long len = h + v;
                    long sum = lh + lv;
                    // best_len / best_sum <= len / sum
                    long difference = best_len * sum - len * best_sum;
                    if (difference < 0) {
                        best_len = len;
                        best_sum = sum;
                        count = 1;
                    } else if (difference == 0) {
                        ++count;
                    }
                }
            }
        }
    }

    long g = gcd(best_sum, best_len);
    best_len /= g;
    best_sum /= g;
    return to_string(best_len) + "/" + to_string(best_sum) + " " + to_string(count);
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
