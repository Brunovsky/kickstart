#undef NDEBUG
#include "../string_search.hpp"

// *****

mt19937 mt(random_device{}());
using strs_t = vector<string>;

ostream& operator<<(ostream& out, const vector<int>& nums) {
    out << '[';
    for (uint i = 0; i < nums.size(); i++)
        out << (i > 0 ? ", " : "") << nums[i];
    return out << "]\n";
}

vector<int> naive_search_all(const string& haystack, const string& needle) {
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

// all strings, first k letters, length [minlen,maxlen]
strs_t generate_all_strings(int letters, int minlen, int maxlen) {
    assert(pow(letters, maxlen) < 1e10);
    strs_t strs{""};
    int L = 0, S = 1, k = 1, splice = 0;
    for (int len = 1; len <= maxlen; len++) {
        if (len == minlen)
            splice = S;
        strs.resize(S + letters * (S - L));
        for (int letter = 0; letter < letters; letter++) {
            char c = 'a' + letter;
            for (int i = L; i < S; i++) {
                strs[k++] = strs[i] + c;
            }
        }
        L = S, S = k;
    }
    strs.erase(begin(strs), begin(strs) + splice);
    return strs;
}

// n random strings, first k letters, length [minlen,maxlen]
strs_t generate_any_strings(int letters, int quantity, int minlen, int maxlen) {
    uniform_int_distribution<int> distlen(minlen, maxlen);
    uniform_int_distribution<char> distchar('a', 'a' + (letters - 1));
    strs_t strs(quantity);
    for (int i = 0; i < quantity; i++) {
        int len = distlen(mt);
        strs[i].resize(len);
        for (int j = 0; j < len; j++)
            strs[i][j] = distchar(mt);
    }
    return strs;
}

void add(strs_t& strs, const strs_t& more) {
    strs.insert(strs.end(), begin(more), end(more));
}

void test() {
    strs_t needles, haystacks;
    add(needles, generate_all_strings(3, 1, 3));
    add(needles, {"abab", "ababa", "ababab", "abababab", "abba", "abbaab", "abbaabba"});
    add(needles, {"cabab", "abcabababc", "ababcabcab", "ababcab", "abcabab"});
    add(needles, {"aabaa", "aaaab", "baaaa", "ababa", "bcabcaabc"});
    add(needles, {"accbcc", "accbccacbc", "accbccaccbcc", "acbcacbc"});
    add(needles, {".", ",!?#:", "aa.bb", "a,", ",a"});
    add(needles, generate_any_strings(3, 37, 6, 15));
    add(needles, generate_any_strings(3, 20, 30, 40));
    add(haystacks, {"...", "#?!", "?!", "   ", "abcdabc", ",bab,", ",aba,"});
    add(haystacks, generate_all_strings(3, 0, 7));
    add(haystacks, generate_any_strings(3, 5'000, 8, 15));
    add(haystacks, generate_any_strings(3, 2'000, 16, 70));
    add(haystacks, generate_any_strings(3, 500, 71, 1000));
    add(haystacks, generate_any_strings(3, 100, 1001, 10000));

    int N = needles.size(), H = haystacks.size();
    printf("# needles: %d\n", N);
    printf("# haystacks: %d\n", H);

    for (auto needle : needles) {
        KMP kmp(needle);
        BoyerMoore bm(needle);
        for (auto haystack : haystacks) {
            auto i1 = naive_search_all(haystack, needle);
            auto i2 = kmp_search_all(haystack, kmp);
            auto i3 = boyer_moore_search_all(haystack, bm);
            assert(i1 == i2);
            assert(i1 == i3);
        }
        printf("OK  %s\n", needle.data());
    }
}

int main() {
    test();
    return 0;
}
