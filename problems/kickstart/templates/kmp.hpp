#include <bits/stdc++.h>

using namespace std;

// *****

namespace kmp {

class KMP {
    vector<int> lookup_;
    string pattern;

public:
    KMP(string pattern) : pattern(pattern) {
        size_t P = pattern.size();

        lookup_.resize(P + 1);

        lookup_[0] = -1;
        int border = 0; // need to compare against 0

        for (size_t index = 1; index < P; ++index, ++border) {
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

    int lookup(size_t index) const {
        return lookup_[index];
    }

    size_t shift(size_t index) const {
        return index - lookup_[index];
    }

    const string &get_pattern() const {
        return pattern;
    }
};

vector<size_t> kmp_search(const string &text, const KMP &kmp) {
    const string &pattern = kmp.get_pattern();
    long P = pattern.size(), T = text.size();

    vector<size_t> match;
    if (P == 0) {
        return match;
    }

    long i = 0;
    long j = 0;

    while (i <= T - P) {
        while ((j < P) && (text[i + j] == pattern[j]))
            ++j;

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

        if (j < 0)
            j = 0;
    }

    return match;
}

} // namespace kmp
