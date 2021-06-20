#pragma once

#include "formatting.hpp"
#include "hash.hpp"
#include "random.hpp"

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

template <typename... Ts>
auto put(ostream& out, Ts&&... args) {
    return print(out, make_format_string(sizeof...(Ts)), std::forward<Ts>(args)...);
}

template <typename... Ts>
auto putln(ostream& out, Ts&&... args) {
    return print(out, make_format_string(sizeof...(Ts)) + "\n",
                 std::forward<Ts>(args)...);
}
