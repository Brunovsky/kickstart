#include "../algo/boyer_moore.hpp"
#include "../algo/kmp.hpp"
#include "../algo/rabin_karp.hpp"
#include "../algo/z_search.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

// *****

/**
 * all strings, with length [minlen,maxlen] with characters [a,b]
 */
auto generate_all_strings(int minlen, int maxlen, char a = 'a', char b = 'z') {
    assert(pow(b - a + 1, maxlen) < 1e8);
    vector<string> strs{""};
    int L = 0, S = 1, k = 1, splice_index = 0;
    for (int len = 1; len <= maxlen; len++) {
        if (len == minlen)
            splice_index = S;
        strs.resize(S + (b - a + 1) * (S - L));
        for (char c = a; c <= b; c++) {
            for (int i = L; i < S; i++) {
                strs[k++] = strs[i] + c;
            }
        }
        L = S, S = k;
    }
    strs.erase(begin(strs), begin(strs) + splice_index);
    return strs;
}

auto naive_search_all(const string& haystack, const string& needle) {
    int P = needle.size(), T = haystack.size();
    vector<int> indices;
    const char* np = needle.data();
    const char* hp = haystack.data();
    for (int i = 0; i + P <= T; i++) {
        if (equal(np, np + P, hp + i))
            indices.push_back(i);
    }
    return indices;
}

inline void add(vector<string>& strs, const vector<string>& more) {
    strs.insert(strs.end(), begin(more), end(more));
}

void stress_test_string_searchers() {
    vector<string> needles, haystacks;

    add(needles, generate_all_strings(1, 6, 'a', 'c'));
    add(needles, {"abab", "ababa", "ababab", "abababab", "abba", "abbaab", "abbaabba"});
    add(needles, {"cabab", "abcabababc", "ababcabcab", "ababcab", "abcabab"});
    add(needles, {"aabaa", "aaaab", "baaaa", "ababa", "bcabcaabc"});
    add(needles, {"accbcc", "accbccacbc", "accbccaccbcc", "acbcacbc"});
    add(needles, {".", ",!?#:", "aa.bb", "a,", ",a"});
    add(needles, generate_any_strings(37, 7, 15, 'a', 'c'));
    add(needles, generate_any_strings(20, 16, 40, 'a', 'c'));

    add(haystacks, {"...", "#?!", "?!", "!?", "   ", "abcdabc", ",bab,", ",aba,"});
    add(haystacks, generate_all_strings(0, 7, 'a', 'c'));
    add(haystacks, generate_any_strings(3'000, 8, 15, 'a', 'c'));
    add(haystacks, generate_any_strings(1'000, 16, 70, 'a', 'c'));
    add(haystacks, generate_any_strings(100, 71, 1000, 'a', 'c'));
    add(haystacks, generate_any_strings(20, 1001, 10000, 'a', 'c'));

    int N = needles.size(), H = haystacks.size();
    print(" no. needles: {}\n", N);
    print(" no. haystacks: {}\n", H);

    for (int i = 0; i < N; i++) {
        print_progress(i, N, "stress test string searchers");
        auto needle = needles[i];
        KMP kmp(needle);
        BoyerMoore bm(needle);
        for (auto haystack : haystacks) {
            auto i1 = naive_search_all(haystack, needle);
            auto i2 = kmp_search_all(haystack, kmp);
            auto i3 = boyer_moore_search_all(haystack, bm);
            auto i4 = rabin_karp_search_all(haystack, needle);
            auto i5 = z_search_all(haystack, needle);
            assert(i1 == i2);
            assert(i1 == i3);
            assert(i1 == i4);
            assert(i1 == i5);
        }
    }
    print_ok("stress test string searchers");
}

int main() {
    stress_test_string_searchers();
    return 0;
}
