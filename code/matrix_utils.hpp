#ifndef MATRIX_UTILS_HPP
#define MATRIX_UTILS_HPP

#include "matrix.hpp"

// *****

template <typename T>
string to_string(const mat<T>& a) {
    vector<vector<string>> cell(a.n, vector<string>(a.m));
    vector<size_t> w(a.m);
    for (int i = 0; i < a.n; i++)
        for (int j = 0; j < a.m; j++)
            cell[i][j] = to_string(a[i][j]), w[j] = max(w[j], cell[i][j].size());
    stringstream ss;
    for (int i = 0; i < a.n; i++) {
        for (int j = 0; j < a.m; j++) {
            ss << setw(w[j] + 1) << cell[i][j];
        }
        ss << '\n';
    }
    return ss.str();
}

template <typename T>
ostream& operator<<(ostream& out, const mat<T>& a) {
    return out << to_string(a);
}

#endif // MATRIX_UTILS_HPP
