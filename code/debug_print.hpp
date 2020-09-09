#ifndef DEBUG_PRINT_HPP
#define DEBUG_PRINT_HPP

#include <bits/stdc++.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;
using fmt::print;
using fmt::format;

// *****

#define DEBUG        1
#define PAD          20
#define FLASH        150
#define debug(name)  print(stderr, "{:>{}}  {}\n", #name, PAD - 2, to_string(name))
#define debugv(name) print(stderr, "{}", join(#name, to_stringv(name)))
#define debugh(name) print(stderr, "{}", join(#name, to_stringh(name)))

constexpr uint dw = 2;
constexpr int dbig = 100, dneg = -10, dnil = -1;
int ddepth = 0, dmul = 4;

string pretty(int n) {
    if (n >= dbig)
        return string(dw, '+');
    if (n <= dneg)
        return string(dw, '-');
    if (n == dnil)
        return string(dw, ' ');
    string s = to_string(n);
    return string(dw - s.size(), ' ') + s;
}

string pretty(long n) {
    if (n >= dbig)
        return string(dw, '+');
    if (n <= dneg)
        return string(dw, '-');
    if (n == dnil)
        return string(dw, ' ');
    string s = to_string(n);
    return string(dw - s.size(), ' ') + s;
}

string pretty(bool b) { return string(dw - 1, ' ') + (b ? '1' : ' '); }

string pretty(string s) {
    if (dw >= s.size())
        return string(dw - s.size(), ' ') + s;
    else
        return s.substr(0, dw);
}

template <typename... Args>
void dprint(Args&&... args) {
    print(stderr, "{}", string(dmul * ddepth, ' '));
    print(stderr, std::forward<Args>(args)...);
}

template <typename... Args>
void dprintin(Args&&... args) {
    print(stderr, "{}", string(dmul * ddepth++, ' '));
    print(stderr, std::forward<Args>(args)...);
}

template <typename... Args>
void dprintout(Args&&... args) {
    print(stderr, "{}", string(dmul * --ddepth, ' '));
    print(stderr, std::forward<Args>(args)...);
}

void diota(string name, int n) {
    print(stderr, "{} {:>{}}    ", string(PAD - 8, '-'), name, 5);
    for (int i = 0; i < n; i++)
        print(stderr, "{:>{}} ", i, dw);
    print(stderr, "\n");
}

void dstep(int i, string name) {
    print(stderr, "{} {}  {}\n", string(PAD + 2, ' '), i, name);
}

void dflash() { print(stderr, "{}\n{}\n", string(FLASH, '='), string(FLASH, '=')); }

namespace std {

template <typename T>
string to_string(const vector<T>& vec) {
    string str;
    str += "{ ";
    for (const auto& e : vec)
        str += pretty(e) + " ";
    str += "}";
    return str;
}

template <typename T>
string to_string(const list<T>& lis) {
    string str;
    str += "( ";
    for (const auto& e : lis)
        str += pretty(e) + " ";
    str += ")";
    return str;
}

template <typename Vec>
vector<string> to_stringv(const vector<Vec>& vec) {
    vector<string> lines;
    size_t n = vec.size(), m = 1;
    for (const auto& row : vec)
        m = max(m, row.size());
    for (uint j = 0; j < m; j++) {
        string line = "{ ";
        for (uint i = 0; i < n; i++)
            if (j < vec[i].size())
                line += pretty(vec[i][j]) + " ";
            else
                line += "   ";
        line += "}";
        lines.push_back(format("{:>{}}: {}", j, dw, line));
    }
    return lines;
}

template <typename Vec>
vector<string> to_stringh(const vector<Vec>& vec) {
    vector<string> lines;
    for (uint i = 0; i < vec.size(); i++)
        if (!vec[i].empty())
            lines.push_back(format("{:>{}}: {}", i, dw, to_string(vec[i])));
    return lines;
}

string join(string name, vector<string>&& vec) {
    constexpr int pad = PAD - dw - 2;
    string str = format("{:>{}}  ", name, pad - 2);
    for (uint i = 0; i < vec.size(); i++) {
        str += format("{}{}\n", string(i ? pad : 0, ' '), vec[i]);
    }
    str += "\n";
    return str;
}

template <typename T>
ostream& operator<<(ostream& out, const vector<T>& vec) {
    return out << to_string(vec);
}

template <typename T>
ostream& operator<<(ostream& out, const list<T>& lis) {
    return out << to_string(lis);
}

} // namespace std

#endif // DEBUG_PRINT_HPP
