#include <bits/stdc++.h>

using namespace std;

// *****

// Knuth Morris Pratt (from library)

namespace {

class KMP {
    vector<int> lookup_;
    string pattern;

public:
    KMP(string pattern);
    int lookup(int index) const;
    int shift(int index) const;
    const string &get_pattern() const;
};

KMP::KMP(string pattern) : pattern(pattern) {
    int P = pattern.size();

    lookup_.resize(P + 1);

    lookup_[0] = -1;
    int border = 0; // need to compare against 0

    for (int index = 1; index < P; ++index, ++border) {
        if (pattern[index] == pattern[border]) {
            lookup_[index] = lookup_[border];
        } else {
            lookup_[index] = border;

            do {
                border = lookup_[border];
            } while (border >= 0 && pattern[index] != pattern[border]);
        }
    }

    lookup_[P] = border;
}

int KMP::lookup(int index) const {
    return lookup_[index];
}

int KMP::shift(int index) const {
    return index - lookup_[index];
}

const string &KMP::get_pattern() const {
    return pattern;
}

vector<int> kmp_search(const string &text, const KMP &kmp) {
    const string &pattern = kmp.get_pattern();
    int P = pattern.size(), T = text.size();

    vector<int> match;
    if (P == 0)
        return match;

    int i = 0;
    int j = 0;

    while (i <= T - P) {
        while ((j < P) && (text[i + j] == pattern[j])) {
            ++j;
        }

        if (j == P) {
            // Matched
            match.push_back(i);
            i += kmp.shift(P);
            j = kmp.lookup(P);
        } else {
            // Mismatched
            i += kmp.shift(j);
            j = kmp.lookup(j);
        }

        if (j < 0) {
            j = 0;
        }
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

void fill_word_counts(const string &word) {
    if (word.empty())
        return;

    KMP kmp(word);
    const int l = word.size();

    for (int r = 1; r <= R; ++r) {
        auto indices = kmp_search(text_rows[r - 1], kmp);
        for (int c : indices) {
            H[r][c + l][l] += long(l);
        }
    }

    for (int c = 1; c <= C; ++c) {
        auto indices = kmp_search(text_cols[c - 1], kmp);
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
    for (int r = 0; r < R; ++r) {
        string text;
        cin >> text >> ws;
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
