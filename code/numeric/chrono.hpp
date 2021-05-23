#ifndef CHRONO_HPP
#define CHRONO_HPP

#include <chrono>

using namespace std::chrono;
using us = microseconds;
using ms = milliseconds;

#define LOOP_FOR_DURATION(max_duration)                  \
    auto loop_start_##__COUNTER__ = steady_clock::now(); \
    while (steady_clock::now() - loop_start_##__COUNTER__ < max_duration)

#define LOOP_FOR_DURATION_OR_RUNS(max_duration, max_runs)               \
    auto loop_start_##__COUNTER__ = steady_clock::now();                \
    for (auto loop_counter_##__COUNTER__ = 0;                           \
         loop_counter_##__COUNTER__ < max_runs &&                       \
         steady_clock::now() - loop_start_##__COUNTER__ < max_duration; \
         loop_counter_##__COUNTER__++)

#define LOOP_FOR_DURATION_TRACKED(max_duration, now)                             \
    auto loop_start_##__COUNTER__ = steady_clock::now();                         \
    for (auto now = decltype(steady_clock::now() - loop_start_##__COUNTER__)(0); \
         now < max_duration; now = steady_clock::now() - loop_start_##__COUNTER__)

#define LOOP_FOR_DURATION_OR_RUNS_TRACKED(max_duration, now, max_runs, run)      \
    auto run = 0;                                                                \
    auto loop_start_##__COUNTER__ = steady_clock::now();                         \
    for (auto now = decltype(steady_clock::now() - loop_start_##__COUNTER__)(0); \
         run < max_runs && now < max_duration;                                   \
         now = steady_clock::now() - loop_start_##__COUNTER__, run++)

#endif // CHRONO_HPP
