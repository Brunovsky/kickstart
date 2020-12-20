#ifndef CHRONO_HPP
#define CHRONO_HPP

#include <fmt/format.h>

#include <chrono>

using namespace std;
using namespace std::chrono;
using us = microseconds;
using fmt::print, fmt::format;

// initialize an accumulator (to 0)
#define START_ACC(var) size_t acc_##var = 0

// start chrono (single or multiple)
#define START(var) auto now_##var = steady_clock::now()

// stop chrono (single)
#define TIME(var) \
    auto time_##var = duration_cast<us>(steady_clock::now() - now_##var).count()

// add chrono (multiple)
#define ADD_TIME(var) \
    acc_##var += duration_cast<us>(steady_clock::now() - now_##var).count()

// output time (single)
#define PRINT_TIME(var) print("\r {:>8}ms -- {}\n", time_##var / 1'000, #var)

// output time (multiple)
#define PRINT_ACC(var) print("\r {:>8}ms -- {}\n", acc_##var / 1'000, #var)

#endif // CHRONO_HPP
