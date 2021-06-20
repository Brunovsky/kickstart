#pragma once

#include <bits/stdc++.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;
using fmt::print;
using fmt::format;

template <typename Seq>
string seq_to_string(const Seq& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}

string align_string_matrix(const vector<vector<string>>& mat) {
    vector<size_t> width;
    for (int i = 0, N = mat.size(); i < N; i++) {
        width.resize(max(width.size(), mat[i].size()));
        for (int j = 0, M = mat[i].size(); j < M; j++) {
            width[j] = max(width[j], mat[i][j].size());
        }
    }
    string s;
    for (int i = 0, N = mat.size(); i < N; i++) {
        for (int j = 0, M = mat[i].size(); j < M; j++) {
            string pad(width[j] - mat[i][j].size(), ' ');
            s += pad + mat[i][j] + " \n"[j + 1 == M];
        }
    }
    return s;
}

template <typename Mat>
string mat_to_string(const Mat& v) {
    vector<vector<string>> string_matrix;
    for (const auto& row : v) {
        string_matrix.emplace_back();
        for (const auto& col : row) {
            if constexpr (std::is_same<decltype(col), const string&>::value) {
                string_matrix.back().push_back(col);
            } else {
                using std::to_string;
                string_matrix.back().push_back(to_string(col));
            }
        }
    }
    return align_string_matrix(string_matrix);
}

template <typename Mat>
string mat_to_string_indices(const Mat& v) {
    vector<vector<string>> string_matrix(1);
    int M = 0;
    for (const auto& row : v) {
        string_matrix.emplace_back();
        string_matrix.back().push_back(std::to_string(string_matrix.size() - 1));
        for (const auto& col : row) {
            if constexpr (std::is_same<decltype(col), const string&>::value) {
                string_matrix.back().push_back(col);
            } else {
                using std::to_string;
                string_matrix.back().push_back(to_string(col));
            }
        }
        M = max(M, int(string_matrix.size()));
    }
    for (int i = 1; i < M; i++) {
        string_matrix[0].push_back(std::to_string(i - 1));
    }
    return align_string_matrix(string_matrix);
}

template <typename U, typename V, typename String>
string format_pair_map(const map<pair<U, V>, String>& times) {
    if (times.empty()) {
        return "";
    }
    set<U> rowset;
    set<V> colset;
    for (const auto& [key, time] : times) {
        rowset.insert(key.first);
        colset.insert(key.second);
    }
    vector<U> rows(begin(rowset), end(rowset));
    vector<V> cols(begin(colset), end(colset));
    int R = rows.size(), C = cols.size();
    vector<vector<string>> string_matrix(R + 1, vector<string>(C + 1));
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (times.count({rows[r], cols[c]})) {
                if constexpr (is_same<String, string>::value) {
                    string_matrix[r + 1][c + 1] = times.at({rows[r], cols[c]});
                } else {
                    string_matrix[r + 1][c + 1] = to_string(times.at({rows[r], cols[c]}));
                }
            }
        }
    }
    for (int r = 0; r < R; r++) {
        using std::to_string;
        if constexpr (is_same<U, string>::value) {
            string_matrix[r + 1][0] = rows[r];
        } else {
            string_matrix[r + 1][0] = to_string(rows[r]);
        }
    }
    for (int c = 0; c < C; c++) {
        using std::to_string;
        if constexpr (is_same<V, string>::value) {
            string_matrix[0][c + 1] = cols[c];
        } else {
            string_matrix[0][c + 1] = to_string(cols[c]);
        }
    }
    return align_string_matrix(string_matrix);
}

template <typename U, typename V, typename W, typename String>
string format_tuple_map(const map<tuple<U, V, W>, String>& times, bool label_row = 0) {
    if (times.empty()) {
        return "";
    }
    set<U> rowset;
    set<V> colset;
    set<W> valset;
    for (const auto& [key, time] : times) {
        rowset.insert(std::get<0>(key));
        colset.insert(std::get<1>(key));
        valset.insert(std::get<2>(key));
    }
    vector<U> rows(begin(rowset), end(rowset));
    vector<V> cols(begin(colset), end(colset));
    vector<W> vals(begin(valset), end(valset));
    int R = rows.size(), C = cols.size(), N = vals.size();
    vector<vector<string>> string_matrix(R + 1 + label_row, vector<string>(C + 1));
    vector<string> val_labels;
    if (label_row) {
        for (int w = 0; w < N; w++) {
            if constexpr (is_same<W, string>::value) {
                val_labels.push_back(vals[w]);
            } else {
                val_labels.push_back(to_string(vals[w]));
            }
        }
    }
    for (int c = 0; c < C; c++) {
        vector<vector<string>> mat(R, vector<string>(N));
        for (int r = 0; r < R; r++) {
            for (int w = 0; w < N; w++) {
                if (times.count({rows[r], cols[c], vals[w]})) {
                    if constexpr (is_same<String, string>::value) {
                        mat[r][w] = times.at({rows[r], cols[c], vals[w]});
                    } else {
                        mat[r][w] = to_string(times.at({rows[r], cols[c], vals[w]}));
                    }
                }
            }
        }
        vector<size_t> width(N);
        for (int r = 0; r < R; r++) {
            for (int w = 0; w < N; w++) {
                width[w] = max(width[w], mat[r][w].size());
            }
        }
        if (label_row) {
            for (int w = 0; w < N; w++) {
                width[w] = max(width[w], val_labels[w].size());
            }
            string_matrix[1][c + 1] = "|";
            for (int w = 0; w < N; w++) {
                string pad(width[w] - val_labels[w].size(), ' ');
                string_matrix[1][c + 1] += " " + pad + val_labels[w];
            }
        }
        for (int r = 0; r < R; r++) {
            string_matrix[r + 1 + label_row][c + 1] = "|";
            for (int w = 0; w < N; w++) {
                string pad(width[w] - mat[r][w].size(), ' ');
                string_matrix[r + 1 + label_row][c + 1] += " " + pad + mat[r][w];
            }
        }
    }
    for (int r = 0; r < R; r++) {
        using std::to_string;
        if constexpr (is_same<U, string>::value) {
            string_matrix[r + 1 + label_row][0] = rows[r];
        } else {
            string_matrix[r + 1 + label_row][0] = to_string(rows[r]);
        }
    }
    for (int c = 0; c < C; c++) {
        using std::to_string;
        if constexpr (is_same<V, string>::value) {
            string_matrix[0][c + 1] = cols[c];
        } else {
            string_matrix[0][c + 1] = to_string(cols[c]);
        }
    }
    return align_string_matrix(string_matrix);
}

template <typename... Ts>
void debugger(string_view vars, Ts&&... args) {
    cerr << ">> [" << vars << "]: ";
    const char* delim = "";
    (..., (cerr << delim << args, delim = ", "));
    cerr << endl;
}
#define debug(...) debugger(#__VA_ARGS__, __VA_ARGS__)

namespace std {

const string& to_string(const string& s) { return s; }

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

template <typename T, typename... Rs>
string to_string(const vector<vector<T, Rs...>>& v) {
    return mat_to_string(v);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const vector<vector<T, Rs...>>& v) {
    return out << to_string(v);
}

template <typename K, typename V, typename... Rs>
string to_string(const map<K, V, Rs...>& v) {
    return seq_to_string(v);
}
template <typename K, typename V, typename... Rs>
ostream& operator<<(ostream& out, const map<K, V, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const deque<T, Rs...>& v) {
    return seq_to_string(v);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const deque<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, size_t N>
string to_string(const array<T, N>& v) {
    return seq_to_string(v);
}

template <typename T, typename... Rs>
string to_string(const list<T, Rs...>& v) {
    return seq_to_string(v);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const list<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const set<T, Rs...>& v) {
    return seq_to_string(v);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const set<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const unordered_set<T, Rs...>& v) {
    return seq_to_string(v);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const unordered_set<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const multiset<T, Rs...>& v) {
    return seq_to_string(v);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const multiset<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const unordered_multiset<T, Rs...>& v) {
    return seq_to_string(v);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const unordered_multiset<T, Rs...>& v) {
    return out << to_string(v);
}

template <typename K, typename V, typename... Rs>
string to_string(const unordered_map<K, V, Rs...>& v) {
    return seq_to_string(v);
}
template <typename K, typename V, typename... Rs>
ostream& operator<<(ostream& out, const unordered_map<K, V, Rs...>& v) {
    return out << to_string(v);
}

template <typename K, typename V, typename... Rs>
string to_string(const multimap<K, V, Rs...>& v) {
    return seq_to_string(v);
}
template <typename K, typename V, typename... Rs>
ostream& operator<<(ostream& out, const multimap<K, V, Rs...>& v) {
    return out << to_string(v);
}

template <typename K, typename V, typename... Rs>
string to_string(const unordered_multimap<K, V, Rs...>& v) {
    return seq_to_string(v);
}
template <typename K, typename V, typename... Rs>
ostream& operator<<(ostream& out, const unordered_multimap<K, V, Rs...>& v) {
    return out << to_string(v);
}

template <std::size_t N, std::size_t... I>
struct gen_indices : gen_indices<(N - 1), (N - 1), I...> {};
template <std::size_t... I>
struct gen_indices<0, I...> : integer_sequence<std::size_t, I...> {};

template <typename H>
std::string& to_string_impl(std::string& s, H&& h) {
    using std::to_string;
    s += to_string(std::forward<H>(h));
    return s;
}

template <typename H, typename... T>
std::string& to_string_impl(std::string& s, H&& h, T&&... t) {
    using std::to_string;
    s += to_string(std::forward<H>(h));
    return to_string_impl(s, std::forward<T>(t)...);
}

template <typename... T, std::size_t... I>
std::string to_string(const std::tuple<T...>& tup, integer_sequence<std::size_t, I...>) {
    std::string result;
    int ctx[] = {(to_string_impl(result, std::get<I>(tup)...), 0), 0};
    (void)ctx;
    return result;
}

template <typename... T>
std::string to_string(const std::tuple<T...>& tup) {
    return to_string(tup, gen_indices<sizeof...(T)>{});
}

} // namespace std
