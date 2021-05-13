#ifndef FORMATTING_HPP
#define FORMATTING_HPP

#include <bits/stdc++.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;
using fmt::print;
using fmt::format;

// cerr << DV(a) << DV(b) << DV(c) << endl;
#define DV(a) (" [" #a "=" + to_string(a) + "]")

namespace std {

template <typename U, typename V>
string to_string(const pair<U, V>& uv) {
    return '(' + to_string(uv.first) + ',' + to_string(uv.second) + ')';
}
template <typename U, typename V>
ostream& operator<<(ostream& out, const pair<U, V>& v) {
    return out << to_string(v);
}

template <typename T>
string to_string(const array<T, 2>& uv) {
    return '(' + to_string(uv[0]) + ',' + to_string(uv[1]) + ')';
}
template <typename T, size_t N>
ostream& operator<<(ostream& out, const array<T, N>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const vector<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const vector<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const vector<vector<T, Rs...>>& v) {
    string s;
    for (const auto& el : v)
        s += "[" + to_string(el) + "] ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const vector<vector<T, Rs...>>& v) {
    return out << to_string(v);
}

template <typename K, typename V, typename... Rs>
string to_string(const map<K, V, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename K, typename V, typename... Rs>
ostream& operator<<(ostream& out, const map<K, V, Rs...>& v) {
    return out << to_string(v);
}

} // namespace std

namespace std {

template <typename T, size_t N>
string to_string(const array<T, N>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}

template <typename T, typename... Rs>
string to_string(const list<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const list<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const set<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const set<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const unordered_set<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const unordered_set<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const multiset<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const multiset<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const unordered_multiset<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const unordered_multiset<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename K, typename V, typename... Rs>
string to_string(const unordered_map<K, V, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename K, typename V, typename... Rs>
ostream& operator<<(ostream& out, const unordered_map<K, V, Rs...>& v) {
    return out << to_string(v);
}

template <typename K, typename V, typename... Rs>
string to_string(const multimap<K, V, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename K, typename V, typename... Rs>
ostream& operator<<(ostream& out, const multimap<K, V, Rs...>& v) {
    return out << to_string(v);
}

template <typename K, typename V, typename... Rs>
string to_string(const unordered_multimap<K, V, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename K, typename V, typename... Rs>
ostream& operator<<(ostream& out, const unordered_multimap<K, V, Rs...>& v) {
    return out << to_string(v);
}

} // namespace std

#endif // FORMATTING_HPP
