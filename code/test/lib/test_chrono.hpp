#ifndef TEST_CHRONO_HPP
#define TEST_CHRONO_HPP

#include "../../numeric/chrono.hpp"

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
#define START_ACC(var)     chrono::nanoseconds time_##var = 0ns
#define START(var)         auto now_##var = chrono::steady_clock::now()
#define CUR_TIME(var)      chrono::steady_clock::now() - now_##var
#define TIME(var)          chrono::nanoseconds time_##var = CUR_TIME(var)
#define ADD_TIME(var)      time_##var += CUR_TIME(var)
#define TIME_S(var)        chrono::duration_cast<chrono::seconds>(time_##var).count()
#define TIME_MS(var)       chrono::duration_cast<chrono::milliseconds>(time_##var).count()
#define TIME_US(var)       chrono::duration_cast<chrono::microseconds>(time_##var).count()
#define FTIME_S(var)       TIME_US(var) / 1e6
#define FTIME_MS(var)      TIME_US(var) / 1e3
#define PRINT_TIME(var)    print_clear(" {:>8}ms -- {}\n", TIME_MS(var), #var)
#define PRINT_TIME_US(var) print_clear(" {:>9}us -- {}\n", TIME_US(var), #var)

#endif // TEST_CHRONO_HPP
