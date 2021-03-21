#include "../dynamic_programming.hpp"

#include "../debug_print.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

// *****

// String distance algorithms

struct distance_test_t {
    string a, b;
    int ans_levenshtein, ans_simple_damerau, ans_damerau;
    int ins = 1, del = 1, sub = 1, tra = 1;
};

// clang-format off
vector<distance_test_t> distance_tests({
    {"mom",          "dad",          3, 3, 3},
    {"abbabab",      "baababab",     2, 2, 2},
    {"abbabab",      "baabbab",      3, 2, 2},
    {"dogs",         "dog",          1, 1, 1},
    {"true",         "true",         0, 0, 0},
    {"true",         "false",        4, 4, 4},
    {"abc",          "ca",           3, 3, 2},
    {"tee",          "tree",         1, 1, 1},
    {"email",        "mails",        2, 2, 2},
    {"profit",       "gifts",        5, 5, 5},
    {"commodo",      "codmomo",      2, 2, 2},
    {"commodo",      "commdABoo",    4, 4, 3},
    {"elementum",    "fermentum",    3, 3, 3},
    {"sitting",      "kitten",       3, 3, 3},
    {"sunday",       "saturday",     3, 3, 3},
    {"aabb...xy",    "abccab...yx",  6, 5, 4},
});
// clang-format on

void unit_test_string_distance() {
    for (int i = 0, T = distance_tests.size(); i < T; i++) {
        auto t = distance_tests[i];
        auto d1 = levenshtein_distance(t.a, t.b, t.ins, t.del, t.sub);
        auto d2 = simple_damerau_distance(t.a, t.b, t.ins, t.del, t.sub, t.tra);
        auto d3 = damerau_distance(t.a, t.b, t.ins, t.del, t.sub, t.tra);
        assert(d1 == t.ans_levenshtein);
        assert(d2 == t.ans_simple_damerau);
        assert(d3 == t.ans_damerau);
    }
}

// Subset sum

auto generate_nums(int n, int min, int max) {
    intd numd(min, max);
    vector<int> nums(n);
    for (int i = 0; i < n; i++)
        nums[i] = numd(mt);
    return nums;
}

auto minmax_sums(const vector<int>& nums) {
    int minsum = 0, maxsum = 0;
    for (int n : nums) {
        n >= 0 ? maxsum += n : minsum += n;
    }
    return array<int, 2>{minsum, maxsum};
}

auto all_sums(const vector<int>& nums) {
    vector<int> sums;
    int N = nums.size();
    for (int i = 0; i < N; i++) {
        int x = nums[i], S = sums.size();
        sums.resize(2 * S + 1);
        for (int j = 0; j < S; j++)
            sums[j + S] = sums[j] + x;
        sums[2 * S] = x;
        sort(begin(sums), end(sums));
        sums.erase(unique(begin(sums), end(sums)), end(sums));
    }
    return sums;
}

void stress_test_subset_sum_run(const vector<int>& nums) {
    auto [min, max] = minmax_sums(nums);
    auto sums = all_sums(nums);
    for (int i = 0, t = min, S = sums.size(); t <= max; t++) {
        if (i < S && t == sums[i]) {
            assert(subset_sum(nums, t)), i++;
        } else {
            assert(!subset_sum(nums, t));
        }
    }
}

void unit_test_subset_sum() {
    vector<int> nums = {17, 63, 49, -71, -23, 84};
    stress_test_subset_sum_run(nums);
    print_ok("unit test subset sum");
}

vector<array<int, 3>> subset_sum_stress_tests = {
    {5, -250, 250},
    {10, -100, 100},
    {25, -60, 60},
    {40, -30, 30},
};

void stress_test_subset_sum(int T = 50) {
    for (auto [n, min, max] : subset_sum_stress_tests) {
        auto label = format("stress test {} {} {}", n, min, max);
        for (int t = 0; t < T; t++) {
            print_progress(t, T, label);
            auto nums = generate_nums(n, min, max);
            stress_test_subset_sum_run(nums);
        }
    }
    print_ok("stress test subset sum");
}

// Knapsack

void unit_test_knapsack() {
    auto [v0, qt0] = repeated_knapsack(15, {1, 4, 1, 2, 12}, {2, 10, 1, 2, 4});
    auto [v1, qt1] = repeated_knapsack(50, {10, 20, 30}, {60, 100, 120});
    auto [v2, qt2] = unit_knapsack(15, {1, 4, 1, 2, 12}, {2, 10, 1, 2, 4});
    auto [v3, qt3] = unit_knapsack(50, {10, 20, 30}, {60, 100, 120});
    assert(v0 == 36 && qt0 == (vector<int>{3, 3, 0, 0, 0}));
    assert(v1 == 300 && qt1 == (vector<int>{5, 0, 0}));
    assert(v2 == 15 && qt2 == (vector<bool>{1, 1, 1, 1, 0}));
    assert(v3 == 220 && qt3 == (vector<bool>{0, 1, 1}));
    print_ok("unit test knapsack");
}

// Subsequences

void unit_test_longest_common_subsequence() {
    auto sub0 = longest_common_subsequence("AGCAT", "GAC");
    auto sub1 = longest_common_subsequence("amputation", "spanking");
    assert(sub0.size() == 2); // GA, AC or GC
    assert(sub1 == "pain");
    print_ok("unit test longest common subsequence");
}

void unit_test_longest_increasing_subsequence() {
    auto sub0 = longest_increasing_subsequence(
        {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15});
    auto sub1 = longest_increasing_subsequence({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    auto sub2 = longest_increasing_subsequence({9, 8, 7, 6, 5, 4, 3, 2, 1, 0});
    auto sub3 = longest_increasing_subsequence({0, 1, 0, 1, 0, 1, 0, 1, 0, 1});
    assert(sub0 == (vector<int>{0, 2, 6, 9, 11, 15}));
    assert(sub1 == (vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    assert(sub2.size() == 1);
    assert(sub3 == (vector<int>{0, 1}));
    print_ok("unit test longest increasing subsequence");
}

int main() {
    unit_test_string_distance();
    unit_test_subset_sum();
    stress_test_subset_sum();
    unit_test_knapsack();
    unit_test_longest_increasing_subsequence();
    return 0;
}
