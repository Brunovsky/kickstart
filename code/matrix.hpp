#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

template <typename T>
struct mat {
    using vec = vector<T>;
    int n, m;
    vector<vec> arr;

    explicit mat(int n = 0, int m = 0) : n(n), m(m), arr(n, vec(m)) {}
    mat(vector<vec> v) : n(v.size()), m(n ? v[0].size() : 0), arr(move(v)) {}

    inline auto& operator[](int x) { return arr[x]; }
    inline const auto& operator[](int x) const { return arr[x]; }
    inline auto& operator()(int x, int y) { return arr[x][y]; }
    inline const auto& operator()(int x, int y) const { return arr[x][y]; }

    bool operator<(const mat& b) const { return tie(n, m, arr) < tie(b.n, b.m, b.arr); }
    bool operator>(const mat& b) const { return b < *this; }
    bool operator<=(const mat& b) const { return !(b < *this); }
    bool operator>=(const mat& b) const { return !(*this < b); }
    bool operator==(const mat& b) const { return n == b.n && m == b.m && arr == b.arr; }
    bool operator!=(const mat& b) const { return !(*this == b); }

    static mat identity(int n) {
        mat a(n, n);
        for (int i = 0; i < n; i++)
            a[i][i] = T(1);
        return a;
    }

    mat operator-(mat a) {
        for (int i = 0; i < a.n; i++)
            for (int j = 0; j < a.m; j++)
                a[i][j] = -a[i][j];
        return a;
    }

    mat& operator+=(const mat& b) {
        assert(n == b.n && m == b.m);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                arr[i][j] += b[i][j];
        return *this;
    }

    mat& operator-=(const mat& b) {
        assert(n == b.n && m == b.m);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                arr[i][j] -= b[i][j];
        return *this;
    }

    friend mat operator+(mat a, const mat& b) { return a += b; }
    friend mat operator-(mat a, const mat& b) { return a -= b; }

    friend mat operator*(const mat& a, const mat& b) {
        assert(a.m == b.n);
        mat c(a.n, b.m);
        for (int i = 0; i < a.n; i++)
            for (int j = 0; j < b.m; j++)
                for (int k = 0; k < a.m; k++)
                    c[i][j] += a[i][k] * b[k][j];
        return c;
    }

    friend mat operator^(mat a, int e) {
        assert(a.n == a.m);
        mat c = identity(a.n);
        while (e > 0) {
            if (e & 1)
                c = c * a;
            if (e >>= 1)
                a = a * a;
        }
        return c;
    }

    friend vec operator*(const mat& a, const vec& b) {
        assert(a.m == int(b.size()));
        vec c(a.m);
        for (int i = 0; i < a.n; i++)
            for (int j = 0; j < a.m; j++)
                c[i] += a[i][j] * b[j];
        return c;
    }

    friend mat transpose(const mat& a) {
        mat b(a.m, a.n);
        for (int i = 0; i < a.m; i++)
            for (int j = 0; j < a.n; j++)
                b[i][j] = a[j][i];
        return b;
    }

    friend mat inverse(mat a) {
        assert(a.n == a.m);
        int n = a.n;
        mat b = identity(n);
        for (int j = 0, i; j < n; j++) {
            for (i = j; i < n; i++) {
                if (abs(a[i][j]) > 1e-10) {
                    swap(a[i], a[j]);
                    break;
                }
            }
            assert(i != n); // degenerate
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

    friend vec gauss(mat a, vec b) {
        assert(a.n == int(b.size()) && a.n == a.m);
        int n = a.n;
        for (int i = 0; i < n; i++)
            a[i].push_back(b[i]);
        for (int j = 0, i; j < n; j++) {
            for (i = j; i < n; i++) {
                if (abs(a[i][j]) > 1e-10) {
                    swap(a[i], a[j]);
                    break;
                }
            }
            assert(i != n);
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
};

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

#endif // MATRIX_HPP
