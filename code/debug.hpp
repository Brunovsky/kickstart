#pragma once

#include <bits/stdc++.h>
using namespace std;

inline namespace lib {

template <typename Seq>
string seq_to_string(const Seq& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}

template <typename Mat>
string mat_to_string(const Mat& v) {
    int N = v.size() + 1;
    vector<vector<string>> cell(1);
    vector<size_t> width(1);
    for (const auto& row : v) {
        int M = row.size(), i = cell.size();
        cell.emplace_back(M + 1, "");
        width.resize(max(int(width.size()), M + 1), 0);
        int j = 1;
        for (const auto& col : row) {
            if constexpr (std::is_same<decltype(col), const string&>::value) {
                cell[i][j] = col;
            } else {
                cell[i][j] = to_string(col);
            }
            width[j] = max(width[j], cell[i][j].size());
            j++;
        }
    }
    int M = width.size();
    cell[0].resize(M);
    for (int i = 1; i < N; i++) {
        cell[i][0] = to_string(i - 1);
        width[0] = max(width[0], cell[i][0].size());
    }
    for (int j = 1; j < M; j++) {
        cell[0][j] = to_string(j - 1);
        width[j] = max(width[j], cell[0][j].size());
    }
    string s;
    for (int i = 0; i < N; i++) {
        for (int S = cell[i].size(), j = 0; j < S; j++) {
            s += string(width[j] + 1 - cell[i][j].size(), ' ') + cell[i][j];
        }
        s += '\n';
    }
    return s;
}

template <typename... Ts>
void debugger(string_view vars, Ts&&... args) {
    cout << ">> [" << vars << "]: ";
    const char* delim = "";
    (..., (cout << delim << args, delim = ", "));
    cout << endl;
}
#define debug(...) debugger(#__VA_ARGS__, __VA_ARGS__)

} // namespace lib

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
    return seq_to_string(v);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const vector<T, Rs...>& v) {
    return out << to_string(v);
}

} // namespace std
