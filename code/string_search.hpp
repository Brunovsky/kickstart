#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Knuth-Morris-Pratt
 * https://en.wikipedia.org/wiki/Knuth-Morris-Pratt_algorithm
 */
class KMP {
    vector<int> table;
    string needle;

  public:
    KMP(string pattern) : needle(move(pattern)) {
        int N = needle.size();
        table.resize(N + 1);
        table[0] = -1;

        int b = 0;
        for (int j = 1; j < N; ++j, ++b) {
            if (needle[j] == needle[b]) {
                table[j] = table[b];
            } else {
                table[j] = b;
                do {
                    b = table[b];
                } while (b >= 0 && needle[j] != needle[b]);
            }
        }
        table[N] = b;
    }

    int lookup(int j) const {
        return j ? table[j] : 0;
    }
    int shift(int j) const {
        return j - table[j];
    }
    const string& get_pattern() const {
        return needle;
    }
};

int kmp_search(const string& text, const KMP& kmp) {
    const string& needle = kmp.get_pattern();
    int N = needle.size(), T = text.size();

    vector<int> match;
    int i = 0, j = 0;

    while (i <= T - N) {
        while (j < N && text[i + j] == needle[j])
            ++j;
        if (j == N)
            return i;
        i += kmp.shift(j);
        j = kmp.lookup(j);
    }

    return -1;
}

vector<int> kmp_search_all(const string& text, const KMP& kmp) {
    const string& needle = kmp.get_pattern();
    int N = needle.size(), T = text.size();

    vector<int> match;
    int i = 0, j = 0;

    while (i <= T - N) {
        while (j < N && text[i + j] == needle[j])
            ++j;
        if (j == N)
            match.push_back(i);
        i += kmp.shift(j);
        j = kmp.lookup(j);
    }

    return match;
}

/**
 * Boyer Moore
 * https://en.wikipedia.org/wiki/Boyer-Moore_string-search_algorithm
 */
class BoyerMoore {
    vector<short> finger;
    vector<vector<int>> bad;
    vector<int> good;
    string needle;

  public:
    BoyerMoore(string pattern) : needle(move(pattern)) {
        int N = needle.size();
        finger.assign(256, -1);
        good.assign(N + 1, 0);

        // bad character rule
        for (int i = 0; i < N; i++) {
            int c = needle[i];
            if (finger[c] == -1) {
                finger[c] = bad.size();
                bad.emplace_back(N, 0);
            }
            for (int j = i + 1; j < N; j++) {
                bad[finger[c]][j] = i;
            }
        }

        // good suffix rule
        vector<int> border(N + 1, 0);
        int b = N + 1;
        border[N] = b;
        for (int i = N; i > 0; i--) {
            while (b <= N && needle[i - 1] != needle[b - 1]) {
                if (good[b] == 0)
                    good[b] = b - i;
                b = border[b];
            }
            border[i - 1] = --b;
        }

        for (int i = 0; i <= N; i++) {
            if (good[i] == 0)
                good[i] = b;
            if (i == b)
                b = border[b];
        }
    }

    int mismatch_shift(int j, char c) const {
        return max(bad_shift(j, c), good_shift(j));
    }
    int match_shift() const {
        return good[0];
    }
    int bad_shift(int j, char c) const {
        return finger[c] == -1 ? j + 1 : j - bad[finger[c]][j];
    }
    int good_shift(int j) const {
        return good[j + 1];
    }
    const string& get_pattern() const {
        return needle;
    }
};

int boyer_moore_search(const string& text, const BoyerMoore& bm) {
    const string& needle = bm.get_pattern();
    int N = needle.size(), T = text.size();

    vector<int> match;
    int i = 0, j = N - 1;

    while (i <= T - N) {
        while ((j >= 0) && (text[i + j] == needle[j]))
            --j;
        if (j < 0)
            return i;
        i += bm.mismatch_shift(j, text[i + j]);
        j = N - 1;
    }

    return -1;
}

vector<int> boyer_moore_search_all(const string& text, const BoyerMoore& bm) {
    const string& needle = bm.get_pattern();
    int N = needle.size(), T = text.size();

    vector<int> match;
    int i = 0, j = N - 1, g = 0;

    while (i <= T - N) {
        while (j >= g && text[i + j] == needle[j])
            --j;
        if (j < g) {
            match.push_back(i);
            i += bm.match_shift();
            g = N - bm.match_shift();
        } else {
            i += bm.mismatch_shift(j, text[i + j]);
            g = 0;
        }
        j = N - 1;
    }

    return match;
}
