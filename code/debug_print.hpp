#ifndef DEBUG_PRINT_HPP
#define DEBUG_PRINT_HPP

#include <bits/stdc++.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;
using fmt::print, fmt::format;

// *****

#define DV(a) ("["s + #a + "=" + to_string(a) + "]")

struct nesting_writer {
    FILE* f;
    int width, depth;
    explicit nesting_writer(FILE* f = stdout, int width = 4, int depth = 0)
        : f(f), width(width), depth(depth) {}

    string pad() const { return string(depth * width, ' '); }
    template <typename... Ts>
    string format(Ts&&... args) const {
        return pad() + fmt::format(forward<Ts>(args)...) + '\n';
    }
    template <typename... Ts>
    void print(Ts&&... args) const {
        print(f, "{}\n", pad() + fmt::format(forward<Ts>(args)...));
    }
    template <typename T>
    auto& operator<<(const T& arg) const {
        return print(f, "{}{}\n", pad(), arg), *this;
    }
};

namespace std {

template <typename U, typename V>
string to_string(const pair<U, V>& uv) {
    return '(' + to_string(uv.first) + ',' + to_string(uv.second) + ')';
}

template <typename T, size_t N>
string to_string(const array<T, N>& v) {
    string s = " ";
    for (const auto& el : v)
        s += to_string(el) + " ";
    return '[' + s + ']';
}
template <typename T>
string to_string(const vector<T>& v) {
    string s = " ";
    for (const auto& el : v)
        s += to_string(el) + " ";
    return '[' + s + ']';
}
template <typename T>
string to_string(const list<T>& v) {
    string s = " ";
    for (const auto& el : v)
        s += to_string(el) + " ";
    return '(' + s + ')';
}
template <typename T>
string to_string(const set<T>& v) {
    string s = " ";
    for (const auto& el : v)
        s += to_string(el) + " ";
    return '{' + s + '}';
}
template <typename T>
string to_string(const unordered_set<T>& v) {
    string s = " ";
    for (const auto& el : v)
        s += to_string(el) + " ";
    return '{' + s + '}';
}
template <typename K, typename V>
string to_string(const map<K, V>& v) {
    string s = " ";
    for (const auto& el : v)
        s += to_string(el) + " ";
    return '{' + s + '}';
}
template <typename K, typename V>
string to_string(const unordered_map<K, V>& v) {
    string s = " ";
    for (const auto& el : v)
        s += to_string(el) + " ";
    return '{' + s + '}';
}

template <typename U, typename V>
ostream& operator<<(ostream& out, const pair<U, V>& v) {
    return out << to_string(v);
}
template <typename T, size_t N>
ostream& operator<<(ostream& out, const array<T, N>& v) {
    return out << to_string(v);
}
template <typename T>
ostream& operator<<(ostream& out, const vector<T>& v) {
    return out << to_string(v);
}
template <typename T>
ostream& operator<<(ostream& out, const list<T>& v) {
    return out << to_string(v);
}
template <typename T>
ostream& operator<<(ostream& out, const set<T>& v) {
    return out << to_string(v);
}
template <typename T>
ostream& operator<<(ostream& out, const unordered_set<T>& v) {
    return out << to_string(v);
}
template <typename K, typename V>
ostream& operator<<(ostream& out, const map<K, V>& v) {
    return out << to_string(v);
}
template <typename K, typename V>
ostream& operator<<(ostream& out, const unordered_map<K, V>& v) {
    return out << to_string(v);
}

} // namespace std

#endif // DEBUG_PRINT_HPP
