#ifndef CHRONO_HPP
#define CHRONO_HPP

#include <chrono>

using namespace std::chrono;
using ns = nanoseconds;
using us = microseconds;
using ms = milliseconds;

#define CONCAT(x, y) x##y

#define LOOP_FOR_DURATION_IMPL(z, max_duration)               \
    auto CONCAT(loop_start, z) = chrono::steady_clock::now(); \
    while (chrono::steady_clock::now() - CONCAT(loop_start, z) < max_duration)

#define LOOP_FOR_DURATION_OR_RUNS_IMPL(z, max_duration, max_runs)            \
    auto CONCAT(loop_start, z) = chrono::steady_clock::now();                \
    for (auto loop_counter_##z = 0;                                          \
         loop_counter_##z < max_runs &&                                      \
         chrono::steady_clock::now() - CONCAT(loop_start, z) < max_duration; \
         loop_counter_##z++)

#define LOOP_FOR_DURATION_TRACKED_IMPL(z, max_duration, now)                          \
    auto CONCAT(loop_start, z) = chrono::steady_clock::now();                         \
    for (auto now = decltype(chrono::steady_clock::now() - CONCAT(loop_start, z))(0); \
         now < max_duration; now = chrono::steady_clock::now() - CONCAT(loop_start, z))

#define LOOP_FOR_DURATION_OR_RUNS_TRACKED_IMPL(z, max_duration, now, max_runs, run)   \
    auto run = 0;                                                                     \
    auto CONCAT(loop_start, z) = chrono::steady_clock::now();                         \
    for (auto now = decltype(chrono::steady_clock::now() - CONCAT(loop_start, z))(0); \
         run < max_runs && now < max_duration;                                        \
         now = chrono::steady_clock::now() - CONCAT(loop_start, z), run++)

#define LOOP_FOR_DURATION(max_duration) LOOP_FOR_DURATION_IMPL(__COUNTER__, max_duration)

#define LOOP_FOR_DURATION_OR_RUNS(max_duration, max_runs) \
    LOOP_FOR_DURATION_OR_RUNS_IMPL(__COUNTER__, max_duration, max_runs)

#define LOOP_FOR_DURATION_TRACKED(max_duration, now) \
    LOOP_FOR_DURATION_TRACKED_IMPL(__COUNTER__, max_duration, now)

#define LOOP_FOR_DURATION_OR_RUNS_TRACKED(max_duration, now, max_runs, run) \
    LOOP_FOR_DURATION_OR_RUNS_TRACKED_IMPL(__COUNTER__, max_duration, now, max_runs, run)

#endif // CHRONO_HPP
