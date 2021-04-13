#ifndef TESTING_HPP
#define TESTING_HPP

#include "formatting.hpp"
#include "random.hpp"

// *****

template <typename T = int>
auto randint(T a, T b) {
    return uniform_int_distribution<T>(a, b)(mt);
}
template <typename T = double>
auto randreal(T a, T b) {
    return uniform_real_distribution<T>(a, b)(mt);
}

string repeat(int n, const string& pat) {
    string s;
    for (int i = 0; i < n; i++)
        s += pat;
    return s;
}
string repeat(int n, const string& pat, const string& sep) {
    string s;
    for (int i = 0; i < n; i++)
        s += (i ? sep : "") + pat;
    return s;
}

string make_format_string(int n) { return repeat(n, "{}", " "); }

template <typename... Ts>
auto put(Ts&&... args) {
    return print(make_format_string(sizeof...(Ts)), std::forward<Ts>(args)...);
}

template <typename... Ts>
auto putln(Ts&&... args) {
    return print(make_format_string(sizeof...(Ts)) + "\n", std::forward<Ts>(args)...);
}

#endif // TESTING_HPP
