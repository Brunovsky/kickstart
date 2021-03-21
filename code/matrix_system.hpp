#ifndef MATRIX_SYSTEM_HPP
#define MATRIX_SYSTEM_HPP

#include "frac.hpp"
#include "matrix.hpp"

// *****

using matd = mat<double>;
using vecd = vector<double>;
using matf = mat<frac>;
using vecf = vector<frac>;

bool mat_zero(frac v) { return v == 0; }
bool mat_zero(double v) { return abs(v) <= 1e-15; }

template <typename T>
optional<mat<T>> inverse(mat<T> a) {
    assert(a.n == a.m && "Matrix inverse operand is not square");
    int n = a.n;
    auto b = mat<T>::identity(n);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (!mat_zero(a[i][j])) {
                swap(a[i], a[j]);
                break;
            }
        }
        if (i == n)
            return {};
        for (int k = 0; k < n; k++)
            b[j][k] /= a[j][j];
        for (int k = n - 1; k >= j; k--)
            a[j][k] /= a[j][j];
        for (i = j + 1; i < n; i++) {
            for (int k = 0; k < n; k++)
                b[i][k] -= a[i][j] * b[j][k];
            for (int k = n - 1; k >= j; k--)
                a[i][k] -= a[i][j] * a[j][k];
        }
    }
    for (int j = n - 1; j >= 0; j--)
        for (int i = 0; i < j; i++)
            for (int k = 0; k < n; k++)
                b[i][k] -= a[i][j] * b[j][k];
    return b;
}

template <typename T>
optional<vector<T>> gauss(mat<T> a, vector<T> b) {
    assert(a.n == b.size() && a.n == a.m);
    int n = a.n;
    for (int i = 0; i < n; i++)
        a[i].push_back(b[i]);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (!mat_zero(a[i][j])) {
                swap(a[i], a[j]);
                break;
            }
        }
        if (i == n)
            return {};
        for (int k = n; k >= j; k--)
            a[j][k] /= a[j][j];
        for (i = j + 1; i < n; i++)
            for (int k = n; k >= j; k--)
                a[i][k] -= a[i][j] * a[j][k];
    }
    for (int j = n - 1; j >= 0; j--)
        for (int i = 0; i < j; i++)
            a[i][n] -= a[i][j] * a[j][n];
    for (int i = 0; i < n; i++)
        b[i] = a[i][n];
    return b;
}

#endif // MATRIX_SYSTEM_HPP
