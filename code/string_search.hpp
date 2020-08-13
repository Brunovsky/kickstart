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

int kmp_search(const string& text, const KMP& kmp) {
    const string& needle = kmp.get_pattern();
    int P = needle.size(), T = text.size();

    vector<int> match;
    int i = 0, j = 0;

    while (i <= T - P) {
        while (j < P && text[i + j] == needle[j]) {
            j++;
        }
        if (j == P) {
            return i;
        }
        i += kmp.shift(j);
        j = kmp.lookup(j);
    }

    return -1;
}

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
        int P = needle.size();
        finger.assign(256, -1);
        good.assign(P + 1, 0);

        // bad character rule
        for (int i = 0; i < P; i++) {
            int c = needle[i];
            if (finger[c] == -1) {
                finger[c] = bad.size();
                bad.emplace_back(P, 0);
            }
            for (int j = i + 1; j < P; j++) {
                bad[finger[c]][j] = i;
            }
        }

        // good suffix rule
        vector<int> border(P + 1, 0);
        int b = P + 1;
        border[P] = b;
        for (int i = P; i > 0; i--) {
            while (b <= P && needle[i - 1] != needle[b - 1]) {
                if (good[b] == 0) {
                    good[b] = b - i;
                }
                b = border[b];
            }
            border[i - 1] = --b;
        }

        for (int i = 0; i <= P; i++) {
            if (good[i] == 0) {
                good[i] = b;
            }
            if (i == b) {
                b = border[b];
            }
        }
    }

    int mismatch_shift(int j, char c) const {
        return max(bad_shift(j, c), good_shift(j));
    }
    int match_shift() const { return good[0]; }
    int bad_shift(int j, char c) const {
        return finger[c] == -1 ? j + 1 : j - bad[finger[c]][j];
    }
    int good_shift(int j) const { return good[j + 1]; }
    const string& get_pattern() const { return needle; }
};

int boyer_moore_search(const string& text, const BoyerMoore& bm) {
    const string& needle = bm.get_pattern();
    int P = needle.size(), T = text.size();

    vector<int> match;
    int i = 0, j = P - 1;

    while (i <= T - P) {
        while ((j >= 0) && (text[i + j] == needle[j])) {
            --j;
        }
        if (j < 0) {
            return i;
        }
        i += bm.mismatch_shift(j, text[i + j]);
        j = P - 1;
    }

    return -1;
}

vector<int> boyer_moore_search_all(const string& text, const BoyerMoore& bm) {
    const string& needle = bm.get_pattern();
    int P = needle.size(), T = text.size();
    int i = 0, j = P - 1, g = 0;
    vector<int> match;

    while (i <= T - P) {
        while (j >= g && text[i + j] == needle[j]) {
            --j;
        }
        if (j < g) {
            match.push_back(i);
            i += bm.match_shift();
            g = P - bm.match_shift();
        } else {
            i += bm.mismatch_shift(j, text[i + j]);
            g = 0;
        }
        j = P - 1;
    }

    return match;
}

/**
 * Rabin Karp
 * https://en.wikipedia.org/wiki/Rabin-Karp_algorithm
 */
struct rolling_hasher {
    static constexpr size_t base = 2001539UL;
    static constexpr size_t mask = (1 << 26) - 1;
    size_t n, mul;

    rolling_hasher(size_t n) : n(n), mul(powovf(base, n) & mask) {}

    size_t operator()(const char* s, const char* e) const noexcept {
        size_t seed = 0;
        while (s != e) {
            seed = (seed * base + *s++) & mask;
        }
        return seed;
    }

    size_t operator()(const string& s) const noexcept {
        return (*this)(s.data(), s.data() + s.length());
    }

    size_t roll(size_t seed, unsigned char out, unsigned char in) const noexcept {
        return (seed * base + in + (mask + 1 - out) * mul) & mask;
    }

    constexpr size_t powovf(size_t base, size_t n) {
        size_t power = 1;
        while (n) {
            if (n & 1) {
                power = power * base;
            }
            n >>= 1;
            base = base * base;
        }
        return power;
    }
};

int rabin_karp_search(const string& text, const string& needle) {
    int T = text.size(), P = needle.size();
    const char *pp = needle.data(), *tp = text.data();
    if (T < P)
        return -1;

    rolling_hasher hash(needle.size());
    size_t ph = hash(pp, pp + P), th = hash(tp, tp + P);

    for (int i = 0; i <= T - P; i++) {
        if (i > 0) {
            th = hash.roll(th, tp[i - 1], tp[i + P - 1]);
        }
        if (ph == th && equal(pp, pp + P, tp + i)) {
            return i;
        }
    }
    return -1;
}

vector<int> rabin_karp_search_all(const string& text, const string& needle) {
    int T = text.size(), P = needle.size();
    const char *pp = needle.data(), *tp = text.data();
    if (T < P)
        return {};

    rolling_hasher hash(needle.size());
    size_t ph = hash(pp, pp + P), th = hash(tp, tp + P);
    vector<int> match;

    for (int i = 0; i <= T - P; i++) {
        if (i > 0) {
            th = hash.roll(th, tp[i - 1], tp[i + P - 1]);
        }
        if (ph == th && equal(pp, pp + P, tp + i)) {
            match.push_back(i);
        }
    }
    return match;
}

/**
 * Z algorithm (prefix length array)
 * https://www.hackerearth.com/practice/algorithms/string-algorithm/z-algorithm/tutorial
 */
int z_search(const string& text, const string& needle, char null = '\0') {
    string s = needle + null + text;
    int P = needle.size(), S = s.size();
    vector<int> z(S);
    int L = 0, R = 0;
    for (int i = 1; i < S; i++) {
        if (i < R && z[i - L] < R - i) {
            z[i] = z[i - L];
        } else {
            L = i;
            R = max(R, i);
            while (R < S && s[R - L] == s[R]) {
                R++;
            }
            z[i] = R - L;
        }
        if (i > P && z[i] == P) {
            return i - (P + 1);
        }
    }
    return -1;
}

vector<int> z_search_all(const string& text, const string& needle, char null = '\0') {
    string s = needle + null + text;
    int P = needle.size(), S = s.size();
    vector<int> z(S);
    int L = 0, R = 0;
    vector<int> match;

    for (int i = 1; i < S; i++) {
        if (i < R && z[i - L] < R - i) {
            z[i] = z[i - L];
        } else {
            L = i;
            R = max(R, i);
            while (R < S && s[R - L] == s[R]) {
                R++;
            }
            z[i] = R - L;
        }
        if (i > P && z[i] == P) {
            match.push_back(i - (P + 1));
        }
    }
    return match;
}
