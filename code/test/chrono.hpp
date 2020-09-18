#ifndef CHRONO_HPP
#define CHRONO_HPP

#include <chrono>

using namespace std::chrono;
using ms = milliseconds;
using us = microseconds;

#define START(var) auto now_##var = steady_clock::now()
#define TIME(var) \
    auto time_##var = duration_cast<ms>(steady_clock::now() - now_##var).count()
#define TIME_SHORT(var) \
    auto time_##var = duration_cast<us>(steady_clock::now() - now_##var).count()
#define CHRONO_PRINT(var)            print("\r {:>8}ms -- {}\n", time_##var, #var)
#define CHRONO_PRINT_NAME(var, name) print("\r {:>8}ms -- {}\n", time_##var, name)

#endif // CHRONO_HPP
