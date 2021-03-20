#ifdef NDEBUG
#undef NDEBUG
#endif

#include "../string_search.hpp"

#include "../gen/strings.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

// *****

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

    add(needles, generate_all_strings(3, 1, 6));
    add(needles, {"abab", "ababa", "ababab", "abababab", "abba", "abbaab", "abbaabba"});
    add(needles, {"cabab", "abcabababc", "ababcabcab", "ababcab", "abcabab"});
    add(needles, {"aabaa", "aaaab", "baaaa", "ababa", "bcabcaabc"});
    add(needles, {"accbcc", "accbccacbc", "accbccaccbcc", "acbcacbc"});
    add(needles, {".", ",!?#:", "aa.bb", "a,", ",a"});
    add(needles, generate_any_strings(37, 3, 7, 15));
    add(needles, generate_any_strings(20, 3, 16, 40));

    add(haystacks, {"...", "#?!", "?!", "!?", "   ", "abcdabc", ",bab,", ",aba,"});
    add(haystacks, generate_all_strings(3, 0, 7));
    add(haystacks, generate_any_strings(3'000, 3, 8, 15));
    add(haystacks, generate_any_strings(1'000, 3, 16, 70));
    add(haystacks, generate_any_strings(100, 3, 71, 1000));
    add(haystacks, generate_any_strings(20, 3, 1001, 10000));

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
