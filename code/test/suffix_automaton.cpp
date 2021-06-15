#include "test_utils.hpp"
#include "../linear/matrix.hpp"
#include "../strings/suffix_automaton.hpp"
#include "../strings/map_suffix_automaton.hpp"
#include "../strings/sparse_suffix_automaton.hpp"
#include "../strings/vector_suffix_automaton.hpp"

void speed_test_build(int min_scale = 10, int max_scale = 18) {
    map<pair<string, int>, string> table;

    for (int scale = min_scale; scale <= max_scale; scale++) {
        START_ACC4(gsa, ssa, msa, vsa);

        LOOP_FOR_DURATION_OR_RUNS_TRACKED (2s, now, 10'000, runs) {
            print_time(now, 2s, "speed test build {}", 1 << scale);
            string s = generate_any_string(1 << scale, 'a', 'z');

            START(gsa);
            suffix_automaton gsa(s);
            ADD_TIME(gsa);

            START(ssa);
            sparse_suffix_automaton ssa(s);
            ADD_TIME(ssa);

            START(msa);
            map_suffix_automaton msa(s);
            ADD_TIME(msa);

            START(vsa);
            vector_suffix_automaton vsa(s);
            ADD_TIME(vsa);
        }

        table[{"gsa", 1 << scale}] = FORMAT_EACH(gsa, runs);
        table[{"ssa", 1 << scale}] = FORMAT_EACH(ssa, runs);
        table[{"msa", 1 << scale}] = FORMAT_EACH(msa, runs);
        table[{"vsa", 1 << scale}] = FORMAT_EACH(vsa, runs);
    }

    print_time_table(table, "Suffix automaton build");
}

void speed_test_contains(int min_scale = 10, int max_scale = 18) {
    map<pair<string, int>, string> table;

    for (int scale = min_scale; scale <= max_scale; scale++) {
        int len = 1 << scale;
        string s = generate_any_string(len, 'a', 'z');

        START_ACC4(gsa, ssa, msa, vsa);

        suffix_automaton gsa(s);
        sparse_suffix_automaton ssa(s);
        map_suffix_automaton msa(s);
        vector_suffix_automaton vsa(s);

        LOOP_FOR_DURATION_OR_RUNS_TRACKED (2s, now, 250'000, runs) {
            print_time(now, 2s, "speed test contains {}/{} {}", scale, max_scale, len);
            string pat = generate_any_string(len / 64, 'a', 'z');

            START(gsa);
            auto c0 = gsa.count_matches(pat);
            ADD_TIME(gsa);

            START(ssa);
            auto c1 = ssa.count_matches(pat);
            ADD_TIME(ssa);

            START(msa);
            auto c2 = msa.count_matches(pat);
            ADD_TIME(msa);

            START(vsa);
            auto c3 = vsa.count_matches(pat);
            ADD_TIME(vsa);

            assert(c0 == c1 && c0 == c2 && c0 == c3);
        }

        table[{"gsa", 1 << scale}] = FORMAT_EACH(gsa, runs);
        table[{"ssa", 1 << scale}] = FORMAT_EACH(ssa, runs);
        table[{"msa", 1 << scale}] = FORMAT_EACH(msa, runs);
        table[{"vsa", 1 << scale}] = FORMAT_EACH(vsa, runs);
    }

    print_time_table(table, "Suffix automaton count matches");
}

template <typename SA = suffix_automaton<>>
void stress_test_suffix_automaton() {
    int errors = 0;
    for (int i = 0; i < 1000; i++) {
        string s = generate_any_string(10000, 'a', 'z');
        SA sa(s);
        for (int j = 0; j < 200; j++) {
            errors += !sa.contains(s.substr(intd(0, 2000)(mt), intd(100, 500)(mt)));
        }
    }
    for (int n = 1; n < 20; n++) {
        string s(n, 'a');
        SA sa(s);
        int got = sa.count_distinct_substrings();
        errors += got != n + 1;
    }
    for (int n = 0; n < 30; n++) {
        for (int m = 1; m < 30; m++) {
            string s = string(n, 'a') + string(m, 'b') + string(n, 'a');
            SA sa(s);
            int got = sa.count_distinct_substrings();
            int expected = n * n + 2 * n * m + m + n + 1;
            errors += got != expected;
        }
    }
    for (int n = 1; n < 25; n++) {
        for (int m = 1; m < 25; m++) {
            for (int k = 1; k < 25; k++) {
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
    for (int n = 1; n < 20; n++) {
        string s(n, '\0');
        iota(begin(s), end(s), 'a');
        SA sa(s);
        int got = sa.count_distinct_substrings();
        errors += got != 1 + n * (n + 1) / 2;
    }
    if (errors > 0) {
        print("ERRORS: {}\n", errors);
    }
}

void unit_test_suffix_automaton() {
    int errors = 0;
    vector_suffix_automaton sa("aabaababaabab"s);
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
    RUN_SHORT(stress_test_suffix_automaton<map_suffix_automaton<>>());
    RUN_SHORT(stress_test_suffix_automaton<vector_suffix_automaton<>>());
    RUN_BLOCK(speed_test_build());
    RUN_BLOCK(speed_test_contains());
    return 0;
}
