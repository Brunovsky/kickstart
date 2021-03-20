#ifndef GEN_STRINGS_HPP
#define GEN_STRINGS_HPP

#include "../random.hpp"

// *****

// all strings, first m letters, length [minlen,maxlen]
auto generate_all_strings(int m, int minlen, int maxlen) {
    assert(pow(m, maxlen) < 1e10);
    vector<string> strs{""};
    int L = 0, S = 1, k = 1, splice_index = 0;
    for (int len = 1; len <= maxlen; len++) {
        if (len == minlen)
            splice_index = S;
        strs.resize(S + m * (S - L));
        for (int letter = 0; letter < m; letter++) {
            char c = 'a' + letter;
            for (int i = L; i < S; i++) {
                strs[k++] = strs[i] + c;
            }
        }
        L = S, S = k;
    }
    strs.erase(begin(strs), begin(strs) + splice_index);
    return strs;
}

// n random strings, first m letters, length [minlen,maxlen]
auto generate_any_strings(int n, int m, int minlen, int maxlen) {
    intd distlen(minlen, maxlen);
    chard distchar('a', 'a' + (m - 1));
    vector<string> strs(n);
    for (int i = 0; i < n; i++) {
        int len = distlen(mt);
        strs[i].resize(len);
        for (int j = 0; j < len; j++)
            strs[i][j] = distchar(mt);
    }
    return strs;
}

#endif // GEN_STRINGS_HPP
