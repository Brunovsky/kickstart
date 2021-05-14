#ifndef TEST_CHRONO_HPP
#define TEST_CHRONO_HPP

#include <chrono>

using namespace std::chrono;
using us = microseconds;
using ms = milliseconds;

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
#define TIME_S(var)        (time_##var / 1e6)
#define TIME_MS(var)       (time_##var / 1'000)
#define TIME_US(var)       time_##var
#define PRINT_TIME(var)    clear_line(), print(" {:>8}ms -- {}\n", TIME_MS(var), #var)
#define PRINT_TIME_US(var) clear_line(), print(" {:>9}us -- {}\n", TIME_US(var), #var)

#endif // TEST_CHRONO_HPP
