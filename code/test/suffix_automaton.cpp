#include "../algo/suffix_automaton.hpp"

#include "../algo/sparse_suffix_automaton.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

template <typename SA = suffix_automaton<>>
void stress_test_suffix_automaton() {
    int errors = 0;
    for (int n = 1; n < 20; n++) {
        string s(n, 'a');
        SA sa(s);
        int got = sa.count_distinct_substrings();
        errors += got != n + 1;
    }
    for (int n = 0; n < 40; n++) {
        for (int m = 1; m < 40; m++) {
            string s = string(n, 'a') + string(m, 'b') + string(n, 'a');
            SA sa(s);
            int got = sa.count_distinct_substrings();
            int expected = n * n + 2 * n * m + m + n + 1;
            errors += got != expected;
        }
    }
    for (int n = 1; n < 30; n++) {
        for (int m = 1; m < 30; m++) {
            for (int k = 1; k < 30; k++) {
                string s = string(n, 'a') + string(m, 'b') + string(n, 'a') +
                           string(k, 'c') + string(n, 'a');
                SA sa(s);
                int got = sa.count_distinct_substrings();
                int expected = 3 * n * (n + k + m) + n + (m + 1) * (k + 1);
                errors += got != expected;
                got = sa.count_matches("aaa");
                expected = 3 * max(n - 2, 0);
                errors += got != expected;
            }
        }
    }
    for (int i = 0; i < 1000; i++) {
        string s = generate_any_string(15000, 'a', 'z');
        SA sa(s);
        errors += !sa.contains(s.substr(19, 73));
        errors += !sa.contains(s.substr(43, 23));
        errors += !sa.contains(s.substr(32, 1));
        errors += !sa.contains(s.substr(17, 89));
        errors += !sa.contains(s.substr(0, 110));
    }
    for (int n = 1; n < 20; n++) {
        string s(n, '\0');
        iota(begin(s), end(s), 'a');
        SA sa(s);
        int got = sa.count_distinct_substrings();
        errors += got != 1 + n * (n + 1) / 2;
    }
    print("errors: {}\n", errors);
}

void unit_test_suffix_automaton() {
    int errors = 0;
    sparse_suffix_automaton sa("aabaababaabab"s);
    errors += (1 != sa.contains("aabaababaabab"));
    errors += (0 != sa.contains("babaababaabab"));
    errors += (1 != sa.contains("abaababaabab"));
    errors += (1 != sa.contains("aabaababaaba"));
    errors += (0 != sa.contains("bb"));
    errors += (1 != sa.contains("babaa"));
    errors += (0 != sa.contains("babaaa"));
    errors += (1 != sa.contains("aabaab"));
    errors += (54 != sa.count_distinct_substrings());
    print("count: {}\n", sa.count_distinct_substrings());
    print("errors: {}\n", errors);
}

int main() {
    RUN_SHORT(unit_test_suffix_automaton());
    RUN_SHORT(stress_test_suffix_automaton<suffix_automaton<>>());
    RUN_SHORT(stress_test_suffix_automaton<sparse_suffix_automaton<>>());
    return 0;
}
