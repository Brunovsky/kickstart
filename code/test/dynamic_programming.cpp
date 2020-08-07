#include "../dynamic_programming.hpp"

// *****

struct distance_test_t {
    string a, b;
    int ans_levenshtein, ans_simple_damerau, ans_damerau;
    int ins = 1, del = 1, sub = 1, tra = 1;
};

vector<distance_test_t> tests({
    {"mom", "dad", 3, 3, 3},
    {"abbabab", "baababab", 2, 2, 2},
    {"abbabab", "baabbab", 3, 2, 2},
    {"dogs", "dog", 1, 1, 1},
    {"true", "true", 0, 0, 0},
    {"true", "false", 4, 4, 4},
    {"abc", "ca", 3, 3, 2},
    {"tee", "tree", 1, 1, 1},
    {"email", "mails", 2, 2, 2},
    {"profit", "gifts", 5, 5, 5},
    {"commodo", "codmomo", 2, 2, 2},
    {"commodo", "commdABoo", 4, 4, 3},
    {"elementum", "fermentum", 3, 3, 3},
    {"sitting", "kitten", 3, 3, 3},
    {"sunday", "saturday", 3, 3, 3},
    {"aabb...xy", "abccab...yx", 6, 5, 4},
});

template <typename T>
ostream& operator<<(ostream& out, const vector<T>& nums) {
    for (int n : nums)
        out << n << ' ';
    return out << endl;
}

vector<int> all_sums(vector<int> nums) {
    vector<int> sums;
    int N = nums.size();
    for (int i = 0; i < N; i++) {
        int x = nums[i];
        int S = sums.size();
        sums.resize(2 * S + 1);
        for (int j = 0; j < S; j++)
            sums[j + S] = sums[j] + x;
        sums[2 * S] = x;
    }
    sort(begin(sums), end(sums));
    sums.erase(unique(begin(sums), end(sums)), end(sums));
    cout << sums;
    return sums;
}

void test_strings() {
    assert(longest_common_subsequence("AGCAT", "GAC").size() == 2UL);
    assert(longest_common_subsequence("amputation", "spanking") == "pain");

    for (uint i = 0; i < tests.size(); i++) {
        auto t = tests[i];
        auto d1 = levenshtein_distance(t.a, t.b, t.ins, t.del, t.sub);
        auto d2 = simple_damerau_distance(t.a, t.b, t.ins, t.del, t.sub, t.tra);
        auto d3 = damerau_distance(t.a, t.b, t.ins, t.del, t.sub, t.tra);
        printf("distance(%s,%s) = %d, %d, %d\n", t.a.data(), t.b.data(), d1, d2, d3);
        assert(d1 == t.ans_levenshtein);
        assert(d2 == t.ans_simple_damerau);
        assert(d3 == t.ans_damerau);
    }
}

void test_subset_sum() {
    vector<int> nums1 = {17, 63, 49, -71, -23, 84};
    auto sums = all_sums(nums1);
    for (int t : sums)
        assert(subset_sum(nums1, t));
    assert(!subset_sum(nums1, 38));
    assert(!subset_sum(nums1, 42));
    assert(!subset_sum(nums1, 8));
    assert(!subset_sum(nums1, 400));
    assert(!subset_sum(nums1, -200));
    assert(!subset_sum(nums1, 111));
}

void test_knapsack() {
    cout << repeated_knapsack(15, {1, 4, 1, 2, 12}, {2, 10, 1, 2, 4});
    cout << unit_knapsack(15, {1, 4, 1, 2, 12}, {2, 10, 1, 2, 4});
    cout << repeated_knapsack(50, {10, 20, 30}, {60, 100, 120});
    cout << unit_knapsack(50, {10, 20, 30}, {60, 100, 120});
}

void test_lis() {
    cout << longest_increasing_subsequence(
        {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15});
    cout << longest_increasing_subsequence({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    cout << longest_increasing_subsequence({9, 8, 7, 6, 5, 4, 3, 2, 1, 0});
    cout << longest_increasing_subsequence({0, 1, 0, 1, 0, 1, 0, 1, 0, 1});
}

int main() {
    test_strings();
    test_subset_sum();
    test_knapsack();
    test_lis();
    return 0;
}
