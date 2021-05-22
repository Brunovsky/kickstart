#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "lib/test_chrono.hpp"
#include "lib/test_progress.hpp"
#include "../random.hpp"

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

#endif // TEST_UTILS_HPP
