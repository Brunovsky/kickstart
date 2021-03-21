#ifndef GEN_STRINGS_HPP
#define GEN_STRINGS_HPP

#include "../random.hpp"

// *****

// all strings, first m letters, length [minlen,maxlen]
auto generate_all_strings(int m, int minlen, int maxlen, char fc = 'a') {
    assert(pow(m, maxlen) < 1e10);
    vector<string> strs{""};
    int L = 0, S = 1, k = 1, splice_index = 0;
    for (int len = 1; len <= maxlen; len++) {
        if (len == minlen)
            splice_index = S;
        strs.resize(S + m * (S - L));
        for (int letter = 0; letter < m; letter++) {
            char c = fc + letter;
            for (int i = L; i < S; i++) {
                strs[k++] = strs[i] + c;
            }
        }
        L = S, S = k;
    }
    strs.erase(begin(strs), begin(strs) + splice_index);
    return strs;
}

auto generate_any_string(int m, int len, char fc = 'a') {
    chard distchar(fc, fc + (m - 1));
    string str(len, 0);
    for (int j = 0; j < len; j++)
        str[j] = distchar(mt);
    return str;
}

// n random strings, first m letters, length [minlen,maxlen]
auto generate_any_strings(int n, int m, int minlen, int maxlen, char fc = 'a') {
    intd distlen(minlen, maxlen);
    vector<string> strs(n);
    for (int i = 0; i < n; i++) {
        strs[i] = generate_any_string(m, distlen(mt), fc);
    }
    return strs;
}

#endif // GEN_STRINGS_HPP
