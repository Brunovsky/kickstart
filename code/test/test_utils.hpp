#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <bits/stdc++.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;
using namespace std::chrono;
using fmt::print, fmt::format;
using us = microseconds;

// *****

void clear_line() { print("\r\033[2K"); }

void print_ok(string line) { clear_line(), print("{} OK\n", line); }

void print_progress(long i, long N) {
    double percent = 100.0 * (i + 1) / N;
    int digits = int(floor(log10(N + .4)));
    print("\033[K\033[s{:5.1f}% {:>{}}/{}\033[u", percent, i + 1, digits, N);
    fflush(stdout);
}

template <typename T>
void print_progress(long i, long N, T&& content) {
    double percent = 100.0 * (i + 1) / N;
    int digits = int(floor(log10(N + .4)));
    string txt = format("{}", forward<T>(content));
    print("\033[K\033[s{:5.1f}% {:>{}}/{} {}\033[u", percent, i + 1, digits, N, txt);
    fflush(stdout);
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

// CHRONO MACROS
// single: To time a portion of code once, do:
//     START($var);
//     run_code();
//     TIME($var);
//     PRINT_TIME($var);
// multiple: To accumulate a portion of a loop, do:
//     START_ACC($var);
//     for (...) {
//         ...
//         START($var);
//         run_code();
//         ADD_TIME($var);
//         ...
//     }
//     PRINT_TIME($var);
// Either way, in the end the result is in time_$var

// initialize an accumulator (multiple)
#define START_ACC(var) size_t time_##var = 0

// start timing (single or multiple)
#define START(var) auto now_##var = steady_clock::now()

// get current time
#define CUR_TIME(var) duration_cast<us>(steady_clock::now() - now_##var)

// stop timing (single)
#define TIME(var) auto time_##var = CUR_TIME(var).count()

// add chrono (multiple)
#define ADD_TIME(var) time_##var += CUR_TIME(var).count()

// get time in milliseconds
#define TIME_MS(var) time_##var / 1'000

// output time (single or multiple)
#define PRINT_TIME(var) clear_line(), print(" {:>8}ms -- {}\n", TIME_MS(var), #var)

#endif // TEST_UTILS_HPP
