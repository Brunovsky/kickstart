#include "test_utils.hpp"
#include "../numeric/fwht.hpp"
#include "../numeric/convolution.hpp"

inline namespace detail {

template <int A, int B, int C, int D, bool reverse = true, typename T>
vector<T> conv_transformed(vector<T> v) {
    conv_transform<A, B, C, D, reverse>(v);
    return v;
}

#define HEX(x) ((x) < 10 ? ('0' + (x)) : ('A' + ((x)-10)))

struct base16 {
    long n;
    constexpr base16(long i = 0) : n(i) {}
    constexpr operator const long&() const { return n; }
    constexpr operator long&() { return n; }

    friend string to_string(base16 num) {
        string hex;
        do {
            hex.push_back(HEX(num.n & 0xf)), num >>= 4;
        } while (num > 0);
        return hex;
    }
    friend ostream& operator<<(ostream& out, base16 num) { return out << to_string(num); }
};

constexpr base16 operator""_k(unsigned long long n) { return 1L << (4 * n); }

template <int A, int B, int C, int D, typename T>
void explore(string what, const vector<T>& a, const vector<T>& b) {
    print("=== ({:2},{:2},{:2},{:2}) {}\n", A, B, C, D, what);
    print("conv_transform(a) = {}\n", conv_transformed<A, B, C, D>(a));
    print("conv_transform(b) = {}\n", conv_transformed<A, B, C, D>(b));
    print("multiply(a,b) = {}\n", convolve<A, B, C, D>(a, b));
}

template <typename T, typename O = T>
auto naive_mobius_subsets(const vector<T>& f) {
    int N = f.size();
    vector<O> F(N);
    for (int mask = 0; mask < N; mask++) {
        for (int submask = 0; submask < N; submask++) {
            if ((submask & mask) == submask && (submask | mask) == mask) {
                int m = mask ? __builtin_popcount(mask) : 0;
                int s = submask ? __builtin_popcount(submask) : 0;
                int i = ((m - s) & 1) ? -1 : 1;
                F[mask] += i * f[submask];
            }
        }
    }
    return F;
}

template <typename T, typename O = T>
auto naive_mobius_supersets(const vector<T>& f) {
    int N = f.size();
    vector<O> F(N);
    for (int mask = 0; mask < N; mask++) {
        for (int submask = 0; submask < N; submask++) {
            if ((submask & mask) == submask && (submask | mask) == mask) {
                int m = mask ? __builtin_popcount(mask) : 0;
                int s = submask ? __builtin_popcount(submask) : 0;
                int i = ((m - s) & 1) ? -1 : 1;
                F[submask] += i * f[mask];
            }
        }
    }
    return F;
}

template <typename T, typename O = T>
auto naive_subset_convolution(const vector<T>& f, const vector<T>& g) {
    int N = f.size();
    vector<O> S(N);
    for (int mask = 0; mask < N; mask++) {
        S[mask] += f[0] * g[mask];
        for (int submask = mask; submask; submask = mask & (submask - 1)) {
            S[mask] += f[submask] * g[mask ^ submask];
        }
    }
    return S;
}

} // namespace detail

void unit_test_convolution() {
    vector<base16> a = {0_k, 1_k, 2_k, 3_k, 4_k, 5_k, 6_k, 7_k};
    vector<base16> b = {1, 1, 1, 1, 2, 1, 4, 1};
    print("a=({})  b=({})\n", a, b);
    print("fwht(a,b) = {}\n", fwht::fwht_convolve(a, b));
    explore<0, 1, 1, 1>("AND", a, b);
    explore<1, 1, 1, 0>("OR", a, b);
    explore<1, 1, 1, -1>("XOR", a, b);
    explore<-1, 1, 1, 1>("REV_XOR", a, b);
}

void unit_test_sos() {
    vector<base16> a = {0_k, 1_k, 2_k, 3_k, 4_k, 5_k, 6_k, 7_k};
    vector<base16> b = {3_k, 4_k, 7_k, 2_k, 1_k, 5_k, 6_k, 0_k};
    auto A = sos_subsets_convolution(a);
    auto B = sos_supersets_convolution(a);
    auto C = mobius_subsets_convolution(a);
    auto D = mobius_supersets_convolution(a);
    auto E = naive_mobius_subsets(a);
    auto F = naive_mobius_supersets(a);
    auto g = naive_mobius_subsets(A);
    auto h = naive_mobius_supersets(B);
    auto G = naive_subset_convolution(a, b);
    auto H = subset_convolution(a, b);
    print("   sos subsets: {}\n", A);
    print("   sos  supers: {}\n", B);
    print("mobius subsets: {}\n", C);
    print("mobius  supers: {}\n", D);
    print(" naive subsets: {}\n", E);
    print(" naive  supers: {}\n", F);
    print("inverse A: {}\n", g);
    print("inverse B: {}\n", h);
    print(" naive conv: {}\n", G);
    print("subset conv: {}\n", H);
}

int main() {
    RUN_SHORT(unit_test_convolution());
    RUN_SHORT(unit_test_sos());
    return 0;
}
