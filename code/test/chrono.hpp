#ifndef CHRONO_HPP
#define CHRONO_HPP

#include <chrono>

using namespace std::chrono;
using ms = milliseconds;
using us = microseconds;

#define START(name) auto now_##name = steady_clock::now()
#define TIME(name) \
    auto time_##name = duration_cast<ms>(steady_clock::now() - now_##name).count()
#define TIME_SHORT(name) \
    auto time_##name = duration_cast<us>(steady_clock::now() - now_##name).count()
#define PRINT(name) print("\r > time " #name ": {}ms\n", time_##name)

#endif // CHRONO_HPP
