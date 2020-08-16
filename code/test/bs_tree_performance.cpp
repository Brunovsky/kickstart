#ifndef NDEBUG
#define NDEBUG
#endif

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <set>

#include "../bs_map.hpp"
#include "../bs_set.hpp"

using namespace std;
using namespace std::chrono;
using ms = chrono::milliseconds;

// *****

template struct bs_set<int>;
template struct bs_set<int, greater<int>>;
template struct bs_multiset<int>;
template struct bs_multiset<int, greater<int>>;
template struct std::set<int>;
template struct std::set<int, greater<int>>;
template struct std::multiset<int>;
template struct std::multiset<int, greater<int>>;

mt19937 mt(random_device{}());
using intd = uniform_int_distribution<int>;
using boold = bernoulli_distribution;

struct unordered_insert_int_test {
    template <typename Set>
    static void run(const string& name, int M, int T, int f) {
        intd distn(0, M);

        auto now = high_resolution_clock::now();
        size_t size = 0;
        for (int t = 1; t <= T; t++) {
            Set set;
            for (int i = 0; i < f * t; i++) {
                set.insert(distn(mt));
            }
            size += set.size();
        }
        auto time = duration_cast<ms>(high_resolution_clock::now() - now);

        double avg_size = 1.0 * size / T;
        printf(" %12s   %ldms  (size: %.1lf)\n", name.data(), time.count(), avg_size);
    }
};

struct ordered_insert_hint_begin_int_test {
    template <typename Set>
    static void run(const string& name, int M, int T, int f) {
        intd distn(0, M);
        vector<int> nums(T * f);
        for (int i = 0; i < T * f; i++) {
            nums[i] = distn(mt);
        }
        sort(begin(nums), end(nums), greater<int>{});

        auto now = high_resolution_clock::now();
        size_t size = 0;
        for (int t = 1; t <= T; t++) {
            Set set;
            for (int i = 0; i < f * t; i++) {
                assert(!i || nums[i] <= *set.begin());
                set.insert(set.begin(), nums[i]);
            }
            size += set.size();
        }
        auto time = duration_cast<ms>(high_resolution_clock::now() - now);

        double avg_size = 1.0 * size / T;
        printf(" %12s   %ldms  (size: %.1lf)\n", name.data(), time.count(), avg_size);
    }
};

struct ordered_insert_hint_end_int_test {
    template <typename Set>
    static void run(const string& name, int M, int T, int f) {
        intd distn(0, M);
        vector<int> nums(T * f);
        for (int i = 0; i < T * f; i++) {
            nums[i] = distn(mt);
        }
        sort(begin(nums), end(nums));

        auto now = high_resolution_clock::now();
        size_t size = 0;
        for (int t = 1; t <= T; t++) {
            Set set;
            for (int i = 0; i < f * t; i++) {
                assert(!i || *set.rbegin() <= nums[i]);
                set.insert(set.end(), nums[i]);
            }
            size += set.size();
        }
        auto time = duration_cast<ms>(high_resolution_clock::now() - now);

        double avg_size = 1.0 * size / T;
        printf(" %12s   %ldms  (size: %.1lf)\n", name.data(), time.count(), avg_size);
    }
};

struct count_query_test {
    template <typename Set>
    static void run(const string& name, int M, int T, int f, int q) {
        intd distn(0, M);
        vector<Set> sets(T);
        for (int t = 0; t < T; t++) {
            for (int i = 0; i < f * t; i++) {
                sets[t].insert(distn(mt));
            }
        }

        auto now = high_resolution_clock::now();
        size_t count = 0;
        for (int t = 0; t < T; t++) {
            for (int i = 0; i < q * t; i++) {
                count += sets[t].count(distn(mt));
            }
        }
        auto time = duration_cast<ms>(high_resolution_clock::now() - now);

        printf(" %12s   %ldms  (count: %lu)\n", name.data(), time.count(), count);
    }
};

struct random_erase_test {
    template <typename Set>
    static void run(const string& name, int M, int T, int f, int e) {
        intd distn(0, M);
        vector<Set> sets(T);
        for (int t = 0; t < T; t++) {
            for (int i = 0; i < f * t; i++) {
                sets[t].insert(distn(mt));
            }
        }

        auto now = high_resolution_clock::now();
        size_t count = 0;
        for (int t = 0; t < T; t++) {
            for (int i = 0; i < e * t; i++) {
                count += sets[t].erase(distn(mt));
            }
        }
        auto time = duration_cast<ms>(high_resolution_clock::now() - now);

        printf(" %12s   %ldms  (count: %lu)\n", name.data(), time.count(), count);
    }
};

struct perfect_unordered_erase_test {
    template <typename Set>
    static void run(const string& name, int M, int T, int f) {
        intd distn(0, M);
        vector<vector<int>> nums(T);
        vector<Set> sets(T);
        for (int t = 0; t < T; t++) {
            nums[t].resize(f * t);
            for (int i = 0; i < f * t; i++) {
                int n = distn(mt);
                nums[t][i] = n;
                sets[t].insert(n);
            }
            // unique set ?
            if (sets[t].size() < size_t(f * t)) {
                sort(begin(nums[t]), end(nums[t]));
                nums[t].erase(unique(begin(nums[t]), end(nums[t])), end(nums[t]));
            }
            shuffle(begin(nums[t]), end(nums[t]), mt);
        }

        auto now = high_resolution_clock::now();
        for (int t = 0; t < T; t++) {
            int len = nums[t].size();
            for (int i = 0; i < len; i++) {
                sets[t].erase(sets[t].find(nums[t][i]));
            }
        }
        auto time = duration_cast<ms>(high_resolution_clock::now() - now);

        printf(" %12s   %ldms\n", name.data(), time.count());
    }
};

struct minmax_test {
    template <typename Set>
    static void run(const string& name, int M, int T, int f, int q) {
        intd distn(0, M);

        auto now = high_resolution_clock::now();
        size_t count = 0;
        long sum = 0;
        for (int t = 0; t < T; t++) {
            Set set;
            for (int i = 0; i < t; i++) {
                for (int j = 0; j < f; j++) {
                    set.insert(distn(mt));
                }
                for (int j = 0; j < q; j++) {
                    count += distn(mt) < *set.begin();
                }
                for (int j = 0; j < q; j++) {
                    count += *set.rbegin() < distn(mt);
                }
                sum += accumulate(begin(set), end(set), 0L);
            }
        }
        auto time = duration_cast<ms>(high_resolution_clock::now() - now);

        printf(" %12s   %ldms  (count: %lu)  (sum: %ld)\n", name.data(), time.count(),
               count, sum);
    }
};

template <typename Fn, typename... Args>
void run(Args&&... args) {
    Fn::template run<set<int>>("set"s, args...);
    Fn::template run<bs_set<int>>("bs_set"s, args...);
    Fn::template run<multiset<int>>("multiset"s, args...);
    Fn::template run<bs_multiset<int>>("bs_multiset"s, args...);
}

int main() {
    printf("# minmax compare and accumulate (int) low inserts -----\n");
    run<minmax_test>(1'000'000, 1000, 2, 50);

    printf("# minmax compare and accumulate (int) many inserts -----\n");
    run<minmax_test>(1'000'000, 500, 10, 20);

    printf("# ordered hinted insertion begin (int) few collisions -----\n");
    run<ordered_insert_hint_begin_int_test>(1'000'000, 2000, 20);

    printf("# ordered hinted insertion begin (int) many collisions -----\n");
    run<ordered_insert_hint_begin_int_test>(1'000, 2000, 20);

    printf("# ordered hinted insertion end (int) few collisions -----\n");
    run<ordered_insert_hint_end_int_test>(1'000'000, 2000, 20);

    printf("# ordered hinted insertion end (int) many collisions -----\n");
    run<ordered_insert_hint_end_int_test>(1'000, 2000, 20);

    printf("# unordered insertion (int) few collisions -----\n");
    run<unordered_insert_int_test>(1'000'000, 1000, 20);

    printf("# unordered insertion (int) moderate collisions -----\n");
    run<unordered_insert_int_test>(25'000, 1000, 20);

    printf("# unordered insertion (int) many collisions -----\n");
    run<unordered_insert_int_test>(1'000, 1000, 20);

    printf("# count query (int) unlikely -----\n");
    run<count_query_test>(1'000'000, 500, 20, 100);

    printf("# count query (int) moderate -----\n");
    run<count_query_test>(25'000, 500, 20, 100);

    printf("# count query (int) likely -----\n");
    run<count_query_test>(1'000, 500, 20, 100);

    printf("# random erase test (int) unlikely -----\n");
    run<random_erase_test>(1'000'000, 500, 20, 100);

    printf("# random erase test (int) moderate -----\n");
    run<random_erase_test>(25'000, 500, 20, 100);

    printf("# random erase test (int) likely -----\n");
    run<random_erase_test>(1'000, 500, 20, 100);

    printf("# perfect unordered erase test (int) -----\n");
    run<perfect_unordered_erase_test>(1'000'000, 1000, 20);

    return 0;
}
