#include <bits/stdc++.h>

using namespace std;

#define long int64_t

// *****

template <typename T>
struct rangenum {
    static_assert(is_integral<T>::value);
    array<T, 2> n = {};

    rangenum() = default;
    rangenum(array<T, 2> ab) : n(ab) {}
    rangenum(T v) : n({v, v}) {}
    rangenum(T a, T b) : n({a, b}) {}

    bool overlaps(rangenum x) const { return n[0] <= x[1] && x[0] <= n[1]; }
    bool contains(rangenum x) const { return n[0] <= x[0] && x[1] <= n[1]; }
    bool empty() const { return n[0] > n[1]; }
    T& operator[](int x) { return n[x]; }
    const T& operator[](int x) const { return n[x]; }

    rangenum rev() const { return rangenum(n[1], n[0]); }
    rangenum& reversed() { return *this = rev(); }

    rangenum operator-() const { return rangenum(-n[0], -n[1]); }
    rangenum operator+() const { return rangenum(n[0], n[1]); }
    rangenum operator++(int) const { return rangenum(n[0]++, n[1]++); }
    rangenum operator--(int) const { return rangenum(n[0]--, n[1]--); }
    rangenum& operator++() const { return ++n[0], ++n[1], *this; }
    rangenum& operator--() const { return --n[0], --n[1], *this; }
    rangenum& operator+=(rangenum v) { return n[0] += v[0], n[1] += v[1], *this; }
    rangenum& operator-=(rangenum v) { return n[0] -= v[0], n[1] -= v[1], *this; }
    rangenum& operator*=(rangenum v) { return n[0] *= v[0], n[1] *= v[1], *this; }
    rangenum& operator/=(rangenum v) { return n[0] /= v[0], n[1] /= v[1], *this; }
    rangenum& operator&=(rangenum v) {
        return n[0] = max(n[0], v[0]), n[1] = min(n[1], v[1]), *this;
    }
    rangenum& operator|=(rangenum v) {
        return n[0] = min(n[0], v[0]), n[1] = max(n[1], v[1]), *this;
    }

    friend rangenum operator+(rangenum lhs, rangenum rhs) { return lhs += rhs; }
    friend rangenum operator-(rangenum lhs, rangenum rhs) { return lhs -= rhs; }
    friend rangenum operator*(rangenum lhs, rangenum rhs) { return lhs *= rhs; }
    friend rangenum operator/(rangenum lhs, rangenum rhs) { return lhs /= rhs; }
    friend rangenum operator&(rangenum lhs, rangenum rhs) { return lhs &= rhs; }
    friend rangenum operator|(rangenum lhs, rangenum rhs) { return lhs |= rhs; }

    friend string to_string(const rangenum& v) {
        return "(" + to_string(v[0]) + "," + to_string(v[1]) + ")";
    }
    friend bool operator==(rangenum lhs, rangenum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(rangenum lhs, rangenum rhs) { return lhs.n != rhs.n; }
    friend bool operator<(rangenum lhs, rangenum rhs) { return lhs.n < rhs.n; }
    friend bool operator>(rangenum lhs, rangenum rhs) { return lhs.n > rhs.n; }
    friend bool operator<=(rangenum lhs, rangenum rhs) { return lhs.n <= rhs.n; }
    friend bool operator>=(rangenum lhs, rangenum rhs) { return lhs.n >= rhs.n; }
    friend ostream& operator<<(ostream& out, rangenum v) { return out << to_string(v); }
    friend istream& operator>>(istream& in, rangenum& v) { return in >> v[0] >> v[1]; }
};

using num = rangenum<long>;

auto solve() {
    long a, b, m;
    cin >> a >> b >> m;
    if (a == b) {
        cout << "1 " << a << '\n';
        return;
    }

    vector<num> range(51), sum(51);
    range[0] = sum[0] = a;
    int i = 0;
    while (range[i][1] < b) {
        range[i + 1] = sum[i] + num(1, m);
        sum[i + 1] = sum[i] + range[i + 1];
        i++;
    }
    if (range[i][0] > b) {
        cout << "-1\n";
        return;
    }
    int N = i + 1;
    vector<long> x(N);
    x[0] = a, x[i--] = b;
    while (i > 0) {
        num parent(x[i + 1] - m, x[i + 1] - 1);
        parent = sum[i] = parent & sum[i] & (sum[i + 1] - x[i + 1]);
        num choices = (parent + num(2, m)) / 2;
        assert(!choices.empty());
        x[i--] = (choices[0] + choices[1]) / 2;
    }
    cout << N << " ";
    for (int j = 0; j < N; j++) {
        cout << x[j] << " \n"[j + 1 == N];
    }
    cout << flush;
    long prefix_sum = a;
    for (int j = 1; j < N; j++) {
        assert(x[j] > prefix_sum);
        assert(x[j] - prefix_sum <= m);
        prefix_sum += x[j];
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
