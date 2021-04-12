#ifndef LINEAR_SYSTEM_HPP
#define LINEAR_SYSTEM_HPP

#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"
#include "matrix.hpp"

// *****

inline namespace system_double {

using matd = mat<double>;
using vecd = vector<double>;

optional<matd> inverse(matd a, double epsilon = 1e-15) {
    assert(a.n == a.m && "Matrix inverse operand is not square");
    int n = a.n;
    auto b = matd::identity(n);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (abs(a[i][j]) > epsilon) {
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

optional<vecd> gauss(matd a, vecd b, double epsilon = 1e-15) {
    assert(a.n == b.size() && a.n == a.m);
    int n = a.n;
    for (int i = 0; i < n; i++)
        a[i].push_back(b[i]);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (abs(a[i][j]) > epsilon) {
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

} // namespace system_double

inline namespace system_frac {

using matf = mat<frac>;
using vecf = vector<frac>;

optional<matf> inverse(matf a) {
    assert(a.n == a.m && "Matrix inverse operand is not square");
    int n = a.n;
    auto b = matf::identity(n);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (a[i][j] != 0) {
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

optional<vecf> gauss(matf a, vecf b) {
    assert(a.n == b.size() && a.n == a.m);
    int n = a.n;
    for (int i = 0; i < n; i++)
        a[i].push_back(b[i]);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (a[i][j] != 0) {
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

} // namespace system_frac

inline namespace system_bfrac {

using matbf = mat<bfrac>;
using vecbf = vector<bfrac>;

optional<matbf> inverse(matbf a) {
    assert(a.n == a.m && "Matrix inverse operand is not square");
    int n = a.n;
    auto b = matbf::identity(n);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (a[i][j] != 0) {
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

optional<vecbf> gauss(matbf a, vecbf b) {
    assert(a.n == b.size() && a.n == a.m);
    int n = a.n;
    for (int i = 0; i < n; i++)
        a[i].push_back(b[i]);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (a[i][j] != 0) {
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

} // namespace system_bfrac

#endif // LINEAR_SYSTEM_HPP
