#include <bits/stdc++.h>

using namespace std;

// *****

class KMP {
    vector<int> lookup_;
    string pattern;

  public:
    KMP(string pattern) : pattern(pattern) {
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

        if (P > 0)
            lookup_[P] = border;
    }

    int lookup(int index) const {
        return lookup_[index];
    }

    int shift(int index) const {
        return index - lookup_[index];
    }

    const string &get_pattern() const {
        return pattern;
    }
};

vector<int> kmp_search(const string &text, const KMP &kmp) {
    const string &pattern = kmp.get_pattern();
    int P = pattern.size(), T = text.size();

    vector<int> match;
    int i = 0, j = 0;

    while (i <= T - P) {
        while (j < P && text[i + j] == pattern[j])
            ++j;
        if (j == P)
            match.push_back(i);
        i += kmp.shift(j);
        j = kmp.lookup(j);
        if (j < 0)
            j = 0;
    }

    return match;
}
