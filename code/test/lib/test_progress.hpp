#pragma once

#include <unistd.h>
#include "test_chrono.hpp"
#include "../../formatting.hpp"

bool cout_is_terminal() {
    static int is = -1;
    return is < 0 ? (is = isatty(STDOUT_FILENO)) : is;
}

void clear_line() { cout_is_terminal() ? print("\r\033[2K") : (void)fflush(stdout); }

template <typename... Ts>
void printcl(Ts&&... args) {
    if (cout_is_terminal()) {
        print("\r\033[2K");
        print(forward<Ts>(args)...);
        fflush(stdout);
    } else {
        print(forward<Ts>(args)...);
        fflush(stdout);
    }
}

void print_progress(long i, long N) {
    if (cout_is_terminal()) {
        double percent = 100.0 * (i + 1) / N;
        int digits = to_string(N).size();
        printcl("{:5.1f}% {:>{}}/{}", percent, i + 1, digits, N);
    }
}

template <typename T>
void print_progress(long i, long N, T&& content) {
    if (cout_is_terminal()) {
        double percent = 100.0 * (i + 1) / N;
        int digits = to_string(N).size();
        string txt = format("{}", forward<T>(content));
        printcl("{:5.1f}% {:>{}}/{} {}", percent, i + 1, digits, N, txt);
    }
}

template <typename... Ts>
void print_progress(long i, long N, string_view fmt, Ts&&... args) {
    return print_progress(i, N, format(fmt, forward<Ts>(args)...));
}

template <typename... Ts>
void print_regular(long i, long N, long step, Ts&&... args) {
    if (cout_is_terminal() && (i == 0 || (i + 1) % step == 0)) {
        print_progress(i, N, forward<Ts>(args)...);
    }
}

template <typename T1, typename T2, typename T>
void print_time_view(T1 now, T2 duration, T&& content) {
    if (cout_is_terminal()) {
        double percent = 100.0 * now / duration;
        auto txt = format("{}", forward<T>(content));
        printcl("{:5.1f}% {}", percent, txt);
    }
}

template <typename T1, typename T2, typename... Ts>
void print_time_view(T1 now, T2 duration, string_view fmt, Ts&&... args) {
    if (cout_is_terminal()) {
        double percent = 100.0 * now / duration;
        auto txt = format(fmt, forward<Ts>(args)...);
        printcl("{:5.1f}% {}", percent, txt);
    }
}

template <typename T1, typename T2, typename... Ts>
void print_time(T1 now, T2 duration, chrono::nanoseconds step, Ts&&... args) {
    static chrono::nanoseconds next_now = 0ns;
    if (cout_is_terminal() && (now == 0ns || now >= next_now)) {
        next_now = now == 0ns ? step : next_now + step;
        print_time_view(now, duration, forward<Ts>(args)...);
    }
}

template <typename... Ts>
[[noreturn]] void fail(Ts&&... args) {
    print("\n"), clear_line();
    print("Error: {}", format(forward<Ts>(args)...));
    exit(1);
}
