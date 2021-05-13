#ifndef TEST_PROGRESS_HPP
#define TEST_PROGRESS_HPP

#include <unistd.h>
#include "../../formatting.hpp"

bool cout_is_terminal() {
    static int ans = -1;
    return ans < 0 ? (ans = isatty(STDOUT_FILENO)) : ans;
}

void clear_line() { cout_is_terminal() ? print("\r\033[2K") : (void)fflush(stdout); }

void print_progress(long i, long N) {
    if (cout_is_terminal()) {
        double percent = 100.0 * (i + 1) / N;
        int digits = to_string(N).size();
        print("\033[K\033[s{:5.1f}% {:>{}}/{}\033[u", percent, i + 1, digits, N);
        fflush(stdout);
    }
}

template <typename T>
void print_progress(long i, long N, T&& content) {
    if (cout_is_terminal()) {
        double percent = 100.0 * (i + 1) / N;
        int digits = to_string(N).size();
        string txt = format("{}", forward<T>(content));
        print("\033[K\033[s{:5.1f}% {:>{}}/{} {}\033[u", percent, i + 1, digits, N, txt);
        fflush(stdout);
    }
}

template <typename... Ts>
void print_progress(long i, long N, string_view fmt, Ts&&... args) {
    return print_progress(i, N, format(fmt, forward<Ts...>(args)...));
}

template <typename... Ts>
[[noreturn]] void fail(Ts&&... args) {
    print("\nError: {}", format(forward<Ts>(args)...));
    exit(1);
}

#endif // TEST_PROGRESS_HPP
