#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <bits/stdc++.h>

using namespace std;

template <typename T>
struct mat {
    using vec = vector<T>;
    int n = 0, m = 0;
    vector<vec> arr;

    mat() = default;
    mat(int n, int m, T v = T()) : n(n), m(m), arr(n, vec(m, v)) {}
    mat(vector<vec> v) : n(v.size()), m(n ? v[0].size() : 0), arr(move(v)) {}

    array<int, 2> size() const { return {n, m}; }
    auto& operator[](int x) { return arr[x]; }
    const auto& operator[](int x) const { return arr[x]; }
    auto& operator[](array<int, 2> x) { return arr[x[0]][x[1]]; }
    const auto& operator[](array<int, 2> x) const { return arr[x[0]][x[1]]; }

    bool operator<(const mat& b) const { return tie(n, m, arr) < tie(b.n, b.m, b.arr); }
    bool operator>(const mat& b) const { return b < *this; }
    bool operator<=(const mat& b) const { return !(b < *this); }
    bool operator>=(const mat& b) const { return !(*this < b); }
    bool operator==(const mat& b) const { return n == b.n && m == b.m && arr == b.arr; }
    bool operator!=(const mat& b) const { return !(*this == b); }

    void resize(int N, int M, T val = T()) {
        if (n != N)
            arr.resize(N, vec(M, val));
        if (m != M)
            for (auto& row : arr)
                row.resize(M, val);
        n = N, m = M;
    }
    void assign(int N, int M, T val = T()) {
        n = N, m = M;
        arr.assign(N, vec(M, val));
    }

    static mat identity(int n) {
        mat a(n, n);
        for (int i = 0; i < n; i++)
            a[i][i] = T(1);
        return a;
    }

    friend mat transpose(const mat& a) {
        mat b(a.m, a.n);
        for (int i = 0; i < a.m; i++)
            for (int j = 0; j < a.n; j++)
                b[i][j] = a[j][i];
        return b;
    }

    friend mat operator-(mat a) {
        for (int i = 0; i < a.n; i++)
            for (int j = 0; j < a.m; j++)
                a[i][j] = -a[i][j];
        return a;
    }

    friend mat& operator+=(mat& a, const mat& b) {
        assert(a.n == b.n && a.m == b.m);
        for (int i = 0; i < a.n; i++)
            for (int j = 0; j < a.m; j++)
                a[i][j] += b[i][j];
        return a;
    }

    friend mat& operator-=(mat& a, const mat& b) {
        assert(a.n == b.n && a.m == b.m);
        for (int i = 0; i < a.n; i++)
            for (int j = 0; j < a.m; j++)
                a[i][j] -= b[i][j];
        return a;
    }

    friend mat operator*(const mat& a, const mat& b) {
        assert(a.m == b.n && "Different matrix mul operand dimensions");
        mat c(a.n, b.m);
        for (int i = 0; i < a.n; i++)
            for (int k = 0; k < a.m; k++)
                for (int j = 0; j < b.m; j++)
                    c[i][j] += a[i][k] * b[k][j];
        return c;
    }

    friend vec operator*(const mat& a, const vec& b) {
        assert(a.m == int(b.size()) && "Matrix and column have unequal dimensions");
        vec c(a.n);
        for (int i = 0; i < a.n; i++)
            for (int j = 0; j < a.m; j++)
                c[i] += a[i][j] * b[j];
        return c;
    }

    friend mat operator^(mat a, long e) {
        assert(a.n == a.m && "Matrix exp operand is not square");
        mat c = identity(a.n);
        while (e > 0) {
            if (e & 1)
                c = c * a;
            if (e >>= 1)
                a = a * a;
        }
        return c;
    }

    friend mat operator+(mat a, const mat& b) { return a += b; }
    friend mat operator-(mat a, const mat& b) { return a -= b; }
    mat operator*=(const mat& b) { return *this = *this * b; }
    mat operator^=(long e) { return *this = *this ^ e; }

    friend string to_string(const mat& a) {
        vector<vector<string>> cell(a.n, vector<string>(a.m));
        vector<size_t> w(a.m);
        for (int i = 0; i < a.n; i++) {
            for (int j = 0; j < a.m; j++) {
                if constexpr (std::is_same_v<T, string>) {
                    cell[i][j] = a[i][j];
                } else {
                    cell[i][j] = to_string(a[i][j]);
                }
                w[j] = max(w[j], cell[i][j].size());
            }
        }
        string s;
        for (int i = 0; i < a.n; i++) {
            for (int j = 0; j < a.m; j++) {
                s += string(w[j] + 1 - cell[i][j].size(), ' ') + cell[i][j];
            }
            s += '\n';
        }
        return s;
    }

    friend ostream& operator<<(ostream& out, const mat& a) { return out << to_string(a); }
};

inline namespace vector_arithmetic {

template <typename T>
vector<T> operator-(vector<T> a) {
    for (int i = 0; i < a.size(); i++)
        a[i] = -a[i];
    return a;
}

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

} // namespace vector_arithmetic

#endif // MATRIX_HPP
