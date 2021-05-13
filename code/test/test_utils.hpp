#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "../random.hpp"
#include "lib/test_chrono.hpp"
#include "lib/test_progress.hpp"

template <typename Container>
bool all_eq(const Container& v) {
    return v.empty() || equal(next(begin(v)), end(v), begin(v));
}

/**
 * googletest vibes now :)
 */
#define RUN_BLOCK(test)                                        \
    do {                                                       \
        print("{:<10} === {}\n", "RUN", #test);                \
        START(runner);                                         \
        test;                                                  \
        TIME(runner);                                          \
        clear_line();                                          \
        print("OK {:>6.2f}s === {}\n", TIME_S(runner), #test); \
    } while (0)

#define RUN_SHORT(test)                                        \
    do {                                                       \
        START(runner);                                         \
        test;                                                  \
        TIME(runner);                                          \
        clear_line();                                          \
        print("OK {:>6.2f}s === {}\n", TIME_S(runner), #test); \
    } while (0)

/**
 * Action selector
 * Each action is given a probability out of an integer total
 */
template <typename Enum = int>
struct action_selector {
    vector<int> steps;

    action_selector(const vector<int>& ratio) : steps(ratio.size()) {
        partial_sum(begin(ratio), end(ratio), begin(steps));
    }

    int total() const { return steps.back(); }
    int options() const { return steps.size(); }
    Enum select() const {
        int k = intd(1, steps.back())(mt);
        return Enum(lower_bound(begin(steps), end(steps), k) - begin(steps));
    }
};

#endif // TEST_UTILS_HPP
