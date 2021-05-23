#include <bits/stdc++.h>

using namespace std;

// *****

namespace fft {

int next_two(int N) { return N > 1 ? 32 - __builtin_clz(N - 1) : 0; }

inline namespace detail {

struct fft_reverse_cache {
    static inline vector<vector<int>> rev;

    static const vector<int>& get(int N) {
        int n = next_two(N);
        for (int r = rev.size(); r <= n; r++) {
            int R = 1 << r;
            rev.emplace_back(R, 0);
            for (int i = 0; i < R; i++) {
                rev[r][i] = (rev[r][i >> 1] | ((i & 1) * R)) >> 1;
            }
        }
        return rev[n];
    }
};

template <typename C>
struct fft_cache {
    static inline vector<C> root = vector<C>(2, 1);
    static inline vector<C> scratch_a, scratch_b;

    static const vector<C>& get_roots(int N) {
        for (int k = root.size(); k < N; k *= 2) {
            root.resize(2 * k);
            C z(cos(M_PI / k), sin(M_PI / k));
            for (int i = k / 2; i < k; i++) {
                root[2 * i] = root[i];
                root[2 * i + 1] = root[i] * z;
            }
        }
        return root;
    }

    static array<C*, 2> get_cache(int N) {
        if (int(scratch_a.size()) < N) {
            scratch_a.resize(N);
            scratch_b.resize(N);
        }
        return {scratch_a.data(), scratch_b.data()};
    }
};

template <bool inverse, typename C>
void fft_transform_run(C* a, int N) {
    auto& rev = fft_reverse_cache::get(N);
    auto& root = fft_cache<C>::get_roots(N);
    for (int i = 0; i < N; i++) {
        if (i < rev[i]) {
            swap(a[i], a[rev[i]]);
        }
    }
    for (int k = 1; k < N; k *= 2) {
        for (int i = 0; i < N; i += 2 * k) {
            for (int l = i, r = i + k, j = 0; j < k; j++, l++, r++) {
                auto z = inverse ? conj(root[j + k]) : root[j + k];
                auto t = a[r] * z;
                a[r] = a[l] - t;
                a[l] = a[l] + t;
            }
        }
    }
    if constexpr (inverse) {
        for (int i = 0; i < N; i++) {
            a[i] /= N;
        }
    }
}

template <typename C, typename PtrA, typename PtrB>
C* fft_multiply_run(PtrA* ia, int A, PtrB* ib, int B) {
    int S = A + B - 1;
    assert(A > 0 && B > 0);
    int N = 1 << next_two(S);
    auto [fa, fb] = fft_cache<C>::get_cache(N);
    copy_n(ia, A, fa);
    fill_n(fa + A, N - A, C(0));
    copy_n(ib, B, fb);
    fill_n(fb + B, N - B, C(0));
    fft_transform_run<0, C>(fa, N); // forward fft A
    fft_transform_run<0, C>(fb, N); // forward fft B
    for (int i = 0; i < N; i++) {
        fa[i] = fa[i] * fb[i]; // multiply A = A * B
    }
    fft_transform_run<1, C>(fa, N); // reverse fft A
    return fa;
}

template <typename C, typename PtrA>
C* fft_square_run(PtrA* ia, int A) {
    int S = 2 * A - 1;
    assert(A > 0);
    int N = 1 << next_two(S);
    auto [fa, fb] = fft_cache<C>::get_cache(N);
    copy_n(ia, A, fa);
    fill_n(fa + A, N - A, C(0));
    fft_transform_run<0, C>(fa, N); // forward fft A
    for (int i = 0; i < N; i++) {
        fa[i] = fa[i] * fa[i]; // multiply A = A * A
    }
    fft_transform_run<1, C>(fa, N); // reverse fft A
    return fa;
}

template <typename C, typename PtrA>
C* fft_inverse_run(PtrA* ia, int A) {
    int S = 2 * A - 1;
    assert(A > 0);
    int N = 1 << next_two(S);
}

template <typename C, typename PtrA, typename PtrB, typename PtrC>
void fft_int_multiply_run(PtrA* ia, int A, PtrB* ib, int B, PtrC* ic) {
    int S = A + B - 1;
    if (A == 0 || B == 0)
        return;
    auto fa = fft_multiply_run<C>(ia, A, ib, B);
    for (int i = 0; i < S; i++) {
        ic[i] = llround(fa[i].real());
    }
}

template <typename C, typename PtrA, typename PtrB, typename PtrC>
void fft_real_multiply_run(PtrA* ia, int A, PtrB* ib, int B, PtrC* ic) {
    int S = A + B - 1;
    if (A == 0 || B == 0)
        return;
    auto fa = fft_multiply_run<C>(ia, A, ib, B);
    for (int i = 0; i < S; i++) {
        ic[i] = fa[i].real();
    }
}

template <typename C, typename PtrA, typename PtrC>
void fft_int_square_run(PtrA* ia, int A, PtrC* ic) {
    int S = 2 * A - 1;
    if (A == 0)
        return;
    auto fa = fft_square_run<C>(ia, A);
    for (int i = 0; i < S; i++) {
        ic[i] = llround(fa[i].real());
    }
}

template <typename C, typename PtrA, typename PtrC>
void fft_real_square_run(PtrA* ia, int A, PtrC* ic) {
    int S = 2 * A - 1;
    if (A == 0)
        return;
    auto fa = fft_square_run<C>(ia, A);
    for (int i = 0; i < S; i++) {
        ic[i] = fa[i].real();
    }
}

} // namespace detail

template <typename T>
vector<T> naive_multiply(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<T> c(S);
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            c[i + j] += a[i] * b[j];
        }
    }
    return c;
}

template <typename T>
vector<T> naive_square(const vector<T>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    vector<T> c(S);
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < A; j++) {
            c[i + j] += a[i] * a[j];
        }
    }
    return c;
}

template <typename C = complex<double>, typename T>
vector<T> fft_multiply(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    if (A < 500 || B < 500) {
        return naive_multiply(a, b);
    }
    vector<T> c(S);
    static_assert(is_integral<T>::value || is_floating_point<T>::value);
    if constexpr (is_integral<T>::value) {
        fft_int_multiply_run<C>(a.data(), A, b.data(), B, c.data());
    } else {
        fft_real_multiply_run<C>(a.data(), A, b.data(), B, c.data());
    }
    return c;
}

template <typename C = complex<double>, typename T>
vector<T> fft_square(const vector<T>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    if (A < 500) {
        return naive_square(a);
    }
    vector<T> c(S);
    static_assert(is_integral<T>::value || is_floating_point<T>::value);
    if constexpr (is_integral<T>::value) {
        fft_int_square_run<C>(a.data(), A, c.data());
    } else {
        fft_real_square_run<C>(a.data(), A, c.data());
    }
    return c;
}

} // namespace fft

auto solve() {
    int N;
    cin >> N;
    vector<long> a(N + 1), b(N + 1);
    for (int i = 0; i <= N; i++) {
        cin >> a[i];
    }
    for (int i = 0; i <= N; i++) {
        cin >> b[i];
    }
    auto c = fft::fft_multiply(a, b);
    for (int i = 0; i <= 2 * N; i++) {
        cout << c[i] << " \n"[i == 2 * N];
    }
}

// *****

int main() {
    ios::sync_with_stdio(false);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
