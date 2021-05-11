#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <unistd.h>

#include "../formatting.hpp"
#include "../random.hpp"

using namespace std::chrono;
using us = microseconds;
using ms = milliseconds;

// *****

bool cout_is_terminal() {
    static int ans = -1;
    return ans < 0 ? (ans = isatty(STDOUT_FILENO)) : ans;
}

void clear_line() { cout_is_terminal() ? print("\r\033[2K") : (void)fflush(stdout); }

void print_progress(long i, long N) {
    if (cout_is_terminal()) {
        double percent = 100.0 * (i + 1) / N;
        int digits = to_string(N).size();
        print("\033[K\033[s{:5.1f}% {:>{}}/{}\033[u", percent, i + 1, digits, N);
        fflush(stdout);
    }
}

template <typename T>
void print_progress(long i, long N, T&& content) {
    if (cout_is_terminal()) {
        double percent = 100.0 * (i + 1) / N;
        int digits = to_string(N).size();
        string txt = format("{}", forward<T>(content));
        print("\033[K\033[s{:5.1f}% {:>{}}/{} {}\033[u", percent, i + 1, digits, N, txt);
        fflush(stdout);
    }
}

template <typename... Ts>
void print_progress(long i, long N, string_view fmt, Ts&&... args) {
    return print_progress(i, N, format(fmt, forward<Ts...>(args)...));
}

template <typename... Ts>
[[noreturn]] void fail(Ts&&... args) {
    print("\nError: {}", format(forward<Ts>(args)...));
    exit(1);
}

template <typename T>
bool all_eq(const vector<T>& v) {
    return equal(begin(v) + 1, end(v), begin(v));
}

/**
 * CHRONO MACROS
 * single: To time a portion of code once, do:
 *     START($var);
 *     run_code();
 *     TIME($var);
 *     PRINT_TIME($var);
 * multiple: To accumulate a portion of a loop, do:
 *     START_ACC($var);
 *     for (...) {
 *         ...
 *         START($var);
 *         run_code();
 *         ADD_TIME($var);
 *         ...
 *     }
 *     PRINT_TIME($var);
 * Either way, in the end the result is in time_$var
 */
#define START_ACC(var)     size_t time_##var = 0
#define START(var)         auto now_##var = steady_clock::now()
#define CUR_TIME(var)      duration_cast<us>(steady_clock::now() - now_##var)
#define TIME(var)          auto time_##var = CUR_TIME(var).count()
#define ADD_TIME(var)      time_##var += CUR_TIME(var).count()
#define TIME_S(var)        time_##var / 1e6
#define TIME_MS(var)       time_##var / 1'000
#define TIME_US(var)       time_##var
#define PRINT_TIME(var)    clear_line(), print(" {:>8}ms -- {}\n", TIME_MS(var), #var)
#define PRINT_TIME_US(var) clear_line(), print(" {:>9}us -- {}\n", TIME_US(var), #var)

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
