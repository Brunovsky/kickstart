#ifndef TEST_PRINT_HPP
#define TEST_PRINT_HPP

#include <bits/stdc++.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;
using fmt::print, fmt::format;

// *****

void clear_line() { print("\r\033[2K"); }

void progress(long i, long N) {
    double percent = 100.0 * (i + 1) / N;
    int digits = int(floor(log10(N + .4)));
    print("\033[K\033[s{:6.1f}% {:>{}}/{}\033[u", percent, i, digits, N);
}

template <typename T>
void progress(long i, long N, T&& content) {
    double percent = 100.0 * (i + 1) / N;
    int digits = int(floor(log10(N + .4)));
    print("\033[K\033[s{:6.1f}% {:>{}}/{} {}\033[u", percent, i, digits, N,
          forward<T>(content));
}

template <typename... Ts>
string format_repeat(string fmt, Ts&&... args) {
    vector<string> strs = {format(fmt, forward<Ts>(args))...};
    return format("{}", fmt::join("{}", strs, " "));
}

template <typename... Ts>
void print_repeat(string fmt, Ts&&... args) {
    vector<string> strs = {format(fmt, forward<Ts>(args))...};
    print("{}", fmt::join("{}", strs, " "));
}

#endif // TEST_PRINT_HPP
