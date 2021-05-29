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
#define START_ACC(var) chrono::nanoseconds time_##var = 0ns
#define START(var)     auto now_##var = chrono::steady_clock::now()
#define ADD_TIME(var)  time_##var += CUR_TIME(var)
#define CUR_TIME(var)  chrono::steady_clock::now() - now_##var
#define TIME(var)      chrono::nanoseconds time_##var = CUR_TIME(var)

#define TIME_MS(var) chrono::duration_cast<chrono::milliseconds>(time_##var).count()
#define TIME_US(var) chrono::duration_cast<chrono::microseconds>(time_##var).count()
#define TIME_NS(var) chrono::duration_cast<chrono::nanoseconds>(time_##var).count()

#define EACH_MS(var, runs) 1e-6 * TIME_NS(var) / runs
#define EACH_US(var, runs) 1e-3 * TIME_NS(var) / runs
#define EACH_NS(var, runs) 1.0 * TIME_NS(var) / runs

#define PRINT_TIME_MS(var) printcl(" {:>8}ms -- {}\n", TIME_MS(var), #var)
#define PRINT_TIME_US(var) printcl(" {:>9}us -- {}\n", TIME_US(var), #var)
#define PRINT_TIME_NS(var) printcl(" {:>10}ns -- {}\n", TIME_US(var), #var)

#define PRINT_EACH_MS(var, runs) printcl(" {:>8.2f}ms -- {}\n", EACH_MS(var, runs), #var)
#define PRINT_EACH_US(var, runs) printcl(" {:>9.1f}us -- {}\n", EACH_US(var, runs), #var)
#define PRINT_EACH_NS(var, runs) printcl(" {:>10.1f}ns -- {}\n", EACH_NS(var, runs), #var)

#endif // TEST_CHRONO_HPP
