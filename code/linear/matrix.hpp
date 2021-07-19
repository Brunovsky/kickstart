#pragma once

#include "../struct/tensor.hpp"

template <typename T>
struct mat : tensor<T, 2> {
    using vec = vector<T>;
    using base_t = tensor<T, 2>;
    using base_t::tensor;
    using base_t::operator[];

    int rows() const { return base_t::shape[0]; }
    int cols() const { return base_t::shape[1]; }
    T* operator[](int x) { return base_t::data + x * base_t::strides[0]; }
    const T* operator[](int x) const { return base_t::data + x * base_t::strides[0]; }

    static mat from(const vector<vector<T>>& vals) {
        int n = vals.size(), m = vals.empty() ? 0u : vals[0].size();
        mat a({n, m});
        for (int i = 0; i < a.rows(); i++) {
            for (int j = 0; j < a.cols(); j++) {
                a[{i, j}] = vals[i][j];
            }
        }
        return a;
    }

    static mat identity(int n) {
        mat a({n, n});
        for (int i = 0; i < n; i++)
            a[{i, i}] = T(1);
        return a;
    }

    friend mat transpose(const mat& a) {
        auto [n, m] = a.size();
        mat b({m, n});
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                b[{i, j}] = a[{j, i}];
        return b;
    }

    friend mat operator-(mat a) {
        auto [n, m] = a.size();
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                a[{i, j}] = -a[{i, j}];
        return a;
    }

    friend mat& operator+=(mat& a, const mat& b) {
        assert(a.size() == b.size());
        auto [n, m] = a.size();
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                a[{i, j}] += b[{i, j}];
        return a;
    }

    friend mat& operator-=(mat& a, const mat& b) {
        assert(a.size() == b.size());
        auto [n, m] = a.size();
        for (int i = 0; i < a.rows(); i++)
            for (int j = 0; j < a.cols(); j++)
                a[{i, j}] -= b[{i, j}];
        return a;
    }

    friend mat operator*(const mat& a, const mat& b) {
        assert(a.cols() == b.rows() && "Different matrix mul operand dimensions");
        int n = a.rows(), p = a.cols(), m = b.cols();
        mat c({n, m});
        for (int i = 0; i < n; i++)
            for (int k = 0; k < p; k++)
                for (int j = 0; j < m; j++)
                    c[{i, j}] += a[{i, k}] * b[{k, j}];
        return c;
    }

    friend vec operator*(const mat& a, const vec& b) {
        assert(a.cols() == int(b.size()) && "Matrix and column have unequal dimensions");
        auto [n, m] = a.size();
        vec c(n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                c[i] += a[{i, j}] * b[j];
        return c;
    }

    friend mat operator^(mat a, long e) {
        assert(a.rows() == a.cols() && "Matrix exp operand is not square");
        mat c = mat::identity(a.rows());
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
        vector<vector<string>> cell(a.rows(), vector<string>(a.cols()));
        vector<size_t> w(a.cols());
        for (int i = 0; i < a.rows(); i++) {
            for (int j = 0; j < a.cols(); j++) {
                if constexpr (std::is_same_v<T, string>) {
                    cell[i][j] = a[{i, j}];
                } else {
                    cell[i][j] = to_string(a[{i, j}]);
                }
                w[j] = max(w[j], cell[i][j].size());
            }
        }
        string s;
        for (int i = 0; i < a.rows(); i++) {
            for (int j = 0; j < a.cols(); j++) {
                s += string(w[j] + 1 - cell[i][j].size(), ' ') + cell[i][j];
            }
            s += '\n';
        }
        return s;
    }

    friend ostream& operator<<(ostream& out, const mat& a) { return out << to_string(a); }
};

template <typename T>
optional<mat<T>> inverse(mat<T> a) {
    assert(a.rows() == a.cols() && "Matrix inverse operand is not square");
    int n = a.rows();
    auto b = mat<T>::identity(n);
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (a[{i, j}]) {
                break;
            }
        }
        if (i == n) {
            return std::nullopt;
        }
        if (i != j) {
            for (int k = 0; k < n; k++) {
                swap(a[{i, k}], a[{j, k}]);
                swap(b[{i, k}], b[{j, k}]);
            }
        }
        for (int k = 0; k < n; k++)
            b[{j, k}] /= a[{j, j}];
        for (int k = n - 1; k >= j; k--)
            a[{j, k}] /= a[{j, j}];
        for (i = j + 1; i < n; i++) {
            if (a[{i, j}]) {
                for (int k = 0; k < n; k++)
                    b[{i, k}] -= a[{i, j}] * b[{j, k}];
                for (int k = n - 1; k >= j; k--)
                    a[{i, k}] -= a[{i, j}] * a[{j, k}];
            }
        }
    }
    for (int j = n - 1; j >= 0; j--)
        for (int i = 0; i < j; i++)
            for (int k = 0; k < n; k++)
                b[{i, k}] -= a[{i, j}] * b[{j, k}];
    return b;
}

template <typename T>
optional<vector<T>> solve_linear_system(mat<T> a, vector<T> b) {
    assert(a.rows() == int(b.size()) && a.rows() == a.cols());
    int n = a.rows();
    for (int j = 0, i; j < n; j++) {
        for (i = j; i < n; i++) {
            if (a[{i, j}]) {
                break;
            }
        }
        if (i == n) {
            return std::nullopt;
        }
        if (i != j) {
            for (int k = 0; k < n; k++) {
                swap(a[{i, k}], a[{j, k}]);
            }
            swap(b[i], b[j]);
        }
        b[j] /= a[{j, j}];
        for (int k = n - 1; k >= j; k--) {
            a[{j, k}] /= a[{j, j}];
        }
        for (i = j + 1; i < n; i++) {
            if (a[{i, j}]) {
                b[i] -= a[{i, j}] * b[j];
                for (int k = n - 1; k >= j; k--) {
                    a[{i, k}] -= a[{i, j}] * a[{j, k}];
                }
            }
        }
    }
    for (int j = n - 1; j >= 0; j--)
        for (int i = 0; i < j; i++)
            b[i] -= a[{i, j}] * b[j];
    return b;
}

template <typename T>
auto gauss_elimination(mat<T>& a) {
    auto [n, m] = a.size();
    int rank = 0;
    T determinant = 1;
    for (int j = 0, i; j < m; j++) {
        for (i = rank; i < n; i++) {
            if (a[{i, j}]) {
                break;
            }
        }
        if (i == n) {
            determinant = 0;
            continue;
        }
        if (i != rank) {
            for (int k = 0; k < m; k++) {
                swap(a[{i, k}], a[{rank, k}]);
            }
            determinant = -determinant;
        }
        determinant *= a[{rank, j}];
        for (int k = m - 1; k >= j + 1; k--) {
            a[{rank, k}] /= a[{rank, j}];
        }
        for (i = 0; i < n; i++) {
            if (i != rank && a[{i, j}]) {
                for (int k = m - 1; k >= j + 1; k--) {
                    a[{i, k}] -= a[{i, j}] * a[{j, k}];
                }
            }
        }
        rank++;
    }
    return make_pair(determinant, rank);
}

template <typename D>
optional<mat<D>> float_inverse(mat<D> a) {
    constexpr D eps = 1e3 * numeric_limits<D>::epsilon();
    assert(a.rows() == a.cols() && "Matrix inverse operand is not square");
    int n = a.rows();
    auto b = mat<D>::identity(n);
    for (int j = 0, i, x; j < n; j++) {
        for (i = x = j; x < n; x++) {
            if (abs(a[{i, j}]) < abs(a[{x, j}])) {
                i = x;
            }
        }
        if (i == n || abs(a[{i, j}] <= eps)) {
            return std::nullopt;
        }
        if (i != j) {
            for (int k = 0; k < n; k++) {
                swap(a[{i, k}], a[{j, k}]);
                swap(b[{i, k}], b[{j, k}]);
            }
        }
        for (int k = 0; k < n; k++)
            b[{j, k}] /= a[{j, j}];
        for (int k = n - 1; k >= j; k--)
            a[{j, k}] /= a[{j, j}];
        for (i = j + 1; i < n; i++) {
            for (int k = 0; k < n; k++)
                b[{i, k}] -= a[{i, j}] * b[{j, k}];
            for (int k = n - 1; k >= j; k--)
                a[{i, k}] -= a[{i, j}] * a[{j, k}];
        }
    }
    for (int j = n - 1; j >= 0; j--)
        for (int i = 0; i < j; i++)
            for (int k = 0; k < n; k++)
                b[{i, k}] -= a[{i, j}] * b[{j, k}];
    return b;
}

template <typename D>
optional<vector<D>> solve_float_linear_system(mat<D> a, vector<D> b) {
    constexpr D eps = 1e3 * numeric_limits<D>::epsilon();
    assert(a.rows() == int(b.size()) && a.rows() == a.cols());
    int n = a.rows();
    for (int j = 0, i, x; j < n; j++) {
        for (i = x = j; x < n; x++) {
            if (abs(a[{i, j}]) < abs(a[{x, j}])) {
                i = x;
            }
        }
        if (i == n || abs(a[{i, j}]) <= eps) {
            return std::nullopt;
        }
        if (i != j) {
            for (int k = 0; k < n; k++) {
                swap(a[{i, k}], a[{j, k}]);
            }
            swap(b[i], b[j]);
        }
        b[j] /= a[{j, j}];
        for (int k = n - 1; k >= j; k--) {
            a[{j, k}] /= a[{j, j}];
        }
        for (i = j + 1; i < n; i++) {
            b[i] -= a[{i, j}] * b[j];
            for (int k = n - 1; k >= j; k--) {
                a[{i, k}] -= a[{i, j}] * a[{j, k}];
            }
        }
    }
    for (int j = n - 1; j >= 0; j--)
        for (int i = 0; i < j; i++)
            b[i] -= a[{i, j}] * b[j];
    return b;
}

/**
 * Gaussian elimination for doubles with partial pivoting
 */
template <typename D>
auto float_gauss_elimination(mat<D>& a) {
    constexpr D eps = 1e3 * numeric_limits<D>::epsilon();
    auto [n, m] = a.size();
    int rank = 0;
    D determinant = 1;
    for (int j = 0, i, x; j < m; j++) {
        for (i = x = rank; x < n; x++) {
            if (abs(a[{i, j}]) < abs(a[{x, j}])) {
                i = x;
            }
        }
        if (i == n || abs(a[{i, j}]) <= eps) {
            determinant = 0;
            continue;
        }
        if (i != rank) {
            for (int k = 0; k < m; k++) {
                swap(a[{i, k}], a[{rank, k}]);
            }
            determinant = -determinant;
        }
        determinant *= a[{rank, j}];
        for (int k = m - 1; k >= j + 1; k--) {
            a[{rank, k}] /= a[{rank, j}];
        }
        for (i = 0; i < n; i++) {
            if (i != rank) {
                for (int k = m - 1; k >= j + 1; k--) {
                    a[{i, k}] -= a[{i, j}] * a[{j, k}];
                }
            }
        }
        rank++;
    }
    return make_pair(determinant, rank);
}
