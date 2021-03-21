#ifndef MATRIX_OPS_HPP
#define MATRIX_OPS_HPP

#include "matrix.hpp"

// *****

template <typename T>
vector<T> operator+(vector<T> a, const vector<T>& b) {
    for (int i = 0; i < a.size(); i++)
        a[i] += b[i];
    return a;
}

template <typename T>
vector<T> operator-(vector<T> a, const vector<T>& b) {
    for (int i = 0; i < a.size(); i++)
        a[i] -= b[i];
    return a;
}

template <typename T>
vector<T> operator*(vector<T> a, const vector<T>& b) {
    for (int i = 0; i < a.size(); i++)
        a[i] *= b[i];
    return a;
}

template <typename T>
void set_row(mat<T>& a, int i, T&& val = T(0)) {
    for (int j = 0; j < a.m; j++)
        a[i][j] = val;
}

template <typename T>
void set_col(mat<T>& a, int j, T&& val = T(0)) {
    for (int i = 0; i < a.n; i++)
        a[i][j] = val;
}

template <typename T>
void add_row(mat<T>& a, int i, T&& add) {
    for (int j = 0; j < a.m; j++)
        a[i][j] += add;
}

template <typename T>
void add_col(mat<T>& a, int j, T&& add) {
    for (int i = 0; i < a.n; i++)
        a[i][j] += add;
}

template <typename T>
void mul_row(mat<T>& a, int i, T&& mul) {
    for (int j = 0; j < a.m; j++)
        a[i][j] *= mul;
}

template <typename T>
void mul_col(mat<T>& a, int j, T&& mul) {
    for (int i = 0; i < a.n; i++)
        a[i][j] *= mul;
}

template <typename T>
void div_row(mat<T>& a, int i, T&& div) {
    for (int j = 0; j < a.m; j++)
        a[i][j] /= div;
}

template <typename T>
void div_col(mat<T>& a, int j, T&& div) {
    for (int i = 0; i < a.n; i++)
        a[i][j] /= div;
}

template <typename T>
void mul_add_row(mat<T>& a, int i, int src, T&& mul) {
    for (int j = 0; j < a.m; j++)
        a[i][j] += mul * a[src][j];
}

template <typename T>
void mul_add_col(mat<T>& a, int j, int src, T&& mul) {
    for (int i = 0; i < a.n; i++)
        a[i][j] += mul * a[i][src];
}

#endif // MATRIX_OPS_HPP
