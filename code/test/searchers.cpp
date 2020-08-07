#undef NDEBUG
#include "../kmp.hpp"

// *****

mt19937 mt(random_device{}());
using strs_t = vector<string>;

ostream& operator<<(ostream& out, const vector<int>& nums) {
    for (int n : nums)
        out << n << ' ';
    return out << endl;
}

/**
 * Naive search, obviously correct
 */
vector<int> naive_search(const string& haystack, const string& needle) {
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

/**
 * generate all strings with first k letters of size up to n (~(k+1)^n strings)
 * careful with input sizes!
 */
strs_t generate_all_strings(int letters, int maxlen) {
    assert(pow(letters, maxlen) < 1e10);
    strs_t strs{""};
    int L = 0, S = 1, k = 1;
    for (int len = 1; len <= maxlen; len++) {
        strs.resize(S + letters * (S - L));
        for (int letter = 0; letter < letters; letter++) {
            char c = 'a' + letter;
            for (int i = L; i < S; i++) {
                strs[k++] = strs[i] + c;
            }
        }
        L = S, S = k;
    }
    return strs;
}

// generate n random strings of length in [minlen,maxlen] with first k letters
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
    add(needles, {"abab", "ababab", "abababab", "abba", "abbaab", "abbaabba"});
    add(needles, {"cabab", "abcabababc", "ababcabcab", "ababcab", "abcabab"});
    add(needles, {"aabaa", "aaaab", "baaaa", "ababa", "bcabcaabc"});
    add(needles, {"", ".", ",!?#:", "aa.bb", "a,", ",a"});
    add(needles, generate_all_strings(3, 3));
    add(needles, generate_any_strings(3, 37, 6, 15));
    add(haystacks, {"...", "#?!", "?!", "   ", "abcdabc", ",bab,", ",aba,"});
    add(haystacks, generate_all_strings(3, 8));
    add(haystacks, generate_any_strings(3, 5'000, 10, 15));
    add(haystacks, generate_any_strings(3, 2'000, 30, 70));
    add(haystacks, generate_any_strings(3, 500, 200, 1000));
    add(haystacks, generate_any_strings(3, 100, 3000, 10000));

    int n, N = needles.size(), H = haystacks.size();
    printf("# needles: %d\n", N);
    printf("# haystacks: %d\n", H);

    n = 0;
    for (auto needle : needles) {
        printf("\rBoyer Moore %2d/%2d %s\n", ++n, N, needle.data());
        auto bm = boyer_moore_searcher(begin(needle), end(needle));
        auto bmh = boyer_moore_horspool_searcher(begin(needle), end(needle));
        for (auto haystack : haystacks) {
            auto i1 = search(begin(haystack), end(haystack), begin(needle), end(needle));
            auto i2 = search(begin(haystack), end(haystack), bm);
            auto i3 = search(begin(haystack), end(haystack), bmh);
            assert(i1 == i2);
            assert(i1 == i3);
        }
    }

    n = 0;
    for (auto needle : needles) {
        printf("\rKMP %2d/%2d %s\n", ++n, N, needle.data());
        KMP kmp(needle);
        for (auto haystack : haystacks) {
            auto i1 = naive_search(haystack, needle);
            auto i2 = kmp_search(haystack, kmp);
            if (i1 != i2) {
                cout << haystack << "   " << needle << '\n' << i2;
            }
            assert(i1 == i2);
        }
    }
}

int main() {
    test();
    return 0;
}
