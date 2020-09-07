#include <boost/multiprecision/cpp_int.hpp>

#include "../bigint.hpp"
#include "../debug_print.hpp"
#include "../random.hpp"

using namespace boost::multiprecision;

// *****

constexpr uint U = UINT_MAX;         // 0xffffffff
constexpr uint M = UINT_MAX / 2 + 1; // 0x80000000

reald distp(0.0, 1.0);
ulongd distv(0, U), distvp(1, U);
intd distn_small(0, 10), distn_pos(1, 8), distn_large(50, 300), distn_any(0, 300);
boold distneg(0.5);

bigint random_bigint(int n) {
    static constexpr double sp[] = {0.15, 0.08, 0.15, 0.07, 0.05};
    static constexpr uint sv[] = {0, 1, U, U - 1, M};
    static constexpr int sk = 5;

    bigint a;
    a.nums.resize(n);
    a.sign = n && distneg(mt);
    for (int i = 0; i < n; i++) {
        double q = distp(mt);
        for (int j = 0; j < sk; j++) {
            q -= sp[j];
            if (q <= 0) {
                a[i] = sv[j];
                break;
            }
        }
        if (q > 0) {
            a[i] = distv(mt);
        }
    }
    if (n > 0 && a[n - 1] == 0)
        a[n - 1] = distvp(mt);
    return a;
}

template <int m>
array<int, m> random_ints(intd& dist) {
    array<int, m> arr;
    for (int i = 0; i < m; i++)
        arr[i] = dist(mt);
    return arr;
}

template <int m>
array<bigint, m> random_bigints(array<int, m> ns) {
    array<bigint, m> arr;
    for (int i = 0; i < m; i++)
        arr[i] = random_bigint(ns[i]);
    return arr;
}

void test_shift() {
    for (int i = 0; i < 128; i++) {
        bigint u = bigint(1) << i;
        print("<< {:3}: {} {}\n", i, u.len(), lsbits(u));
    }
    for (int i = 0; i < 128; i++) {
        bigint u = bigint(1) << i;
        print("<< {:3}: {} {}\n", i, u.len(), msbits(u));
    }
    for (int i = 0; i < 128; i++) {
        bigint u = (bigint(1) << 127) >> i;
        print(">> {:3}: {} {}\n", i, u.len(), lsbits(u));
    }
    bigint v("101011101100001101010101000001101100001111110101", 2);
    for (int i = 0; i < 128; i++) {
        bigint u = v << i;
        print("<< {:3}: {} {}\n", i, u.len(), lsbits(u));
    }
    for (int i = 0; i < 140; i++) {
        bigint u = (v << 127) >> i;
        print("<< {:3}: {} {}\n", i, u.len(), lsbits(u));
    }
}

void test_add_commutative(int R) {
    mt.seed(73);
    for (int i = 1; i <= R; i++) {
        print("\radd commutative {}...", i);

        auto [a, b] = random_bigints<2>(random_ints<2>(distn_small));
        bigint c = a + b;
        bigint d = b + a;

        if (c != d) {
            print("\na: {}\nb: {}\nc: {}\nd: {}\n", a, b, c, d);
        }
        assert(c == d);
    }
    print("\n");
}

void test_add_transitive(int R) {
    for (int i = 1; i <= R; i++) {
        print("\radd transitive {}...", i);

        auto [a, b, c] = random_bigints<3>(random_ints<3>(distn_small));
        bigint d = (a + b) + c;
        bigint e = a + (b + c);

        if (d != e) {
            print("\na: {}\nb: {}\nc: {}\nd: {}\ne: {}\n", a, b, c, d, e);
        }
        assert(d == e);
    }
    print("\n");
}

void test_add_sub_reverse(int R) {
    for (int i = 1; i <= R; i++) {
        print("\radd sub reverse {}...", i);

        auto [a, b] = random_bigints<2>(random_ints<2>(distn_small));
        bigint c = a - b;
        bigint d = b + c;

        if (a != d) {
            print("\na: {}\nb: {}\nc: {}\nd: {}\n", a, b, c, d);
        }
        assert(a == d);
    }
    print("\n");
}

void test_add_sub_group(int R) {
    for (int i = 1; i <= R; i++) {
        print("\radd sub group {}...", i);

        auto [a, b, c] = random_bigints<3>(random_ints<3>(distn_small));
        bigint d = a - b + c;
        bigint e = a - (b - c);
        bigint f = a - b - c;
        bigint g = a - (b + c);

        if (d != e || f != g) {
            print("\na: {}\nb: {}\nc: {}\n", a, b, c);
            print("d: {}\ne: {}\nf: {}\ng: {}\n", d, e, f, g);
        }
        assert(d == e && f == g);
    }
    print("\n");
}

void test_mul_commutative(int R) {
    for (int i = 1; i <= R; i++) {
        print("\rmul commutative {}...", i);

        auto [a, b] = random_bigints<2>(random_ints<2>(distn_small));
        bigint c = a * b;
        bigint d = b * a;

        if (c != d) {
            print("\na: {}\nb: {}\nc: {}\nd: {}\n", a, b, c, d);
        }
        assert(c == d);
    }
    print("\n");
}

void test_mul_transitive(int R) {
    for (int i = 1; i <= R; i++) {
        print("\rmul transitive {}...", i);

        auto [a, b, c] = random_bigints<3>(random_ints<3>(distn_small));
        bigint d = (a * b) * c;
        bigint e = a * (b * c);

        if (d != e) {
            print("\na: {}\nb: {}\nc: {}\nd: {}\ne: {}\n", a, b, c, d, e);
        }
        assert(d == e);
    }
    print("\n");
}

void test_mul_distributive(int R) {
    for (int i = 1; i <= R; i++) {
        print("\rmul distributive {}...", i);

        auto [a, b, c] = random_bigints<3>(random_ints<3>(distn_small));
        bigint d = a * (b + c);
        bigint e = a * b + a * c;

        if (d != e) {
            print("\na: {}\nb: {}\nc: {}\nd: {}\ne: {}\n", a, b, c, d, e);
            print("b+c: {}\na*b: {}\na*c: {}\n", b + c, a * b, a * c);
        }
        assert(d == e);
    }
    print("\n");
}

void test_div_perfect(int R) {
    for (int i = 1; i <= R; i++) {
        print("\rdiv perfect {}...", i);

        auto [a, b] = random_bigints<2>(random_ints<2>(distn_pos));
        bigint c = a * b;
        bigint d = c / a;

        if (d != b) {
            print("\na: {}\nb: {}\nc: {}\nd: {}\n", a, b, c, d);
        }
        assert(d == b);
    }
    print("\n");
}

void test_div_imperfect(int R) {
    for (int i = 1; i <= R; i++) {
        print("\rdiv imperfect {}...", i);

        auto [a, b] = random_bigints<2>(random_ints<2>(distn_pos));
        bigint q = a;
        bigint r = div_mod(q, b);

        if (q * b + r != a) {
            print("\na: {}\nb: {}\nq: {}\nr: {}\n", a, b, q, r);
        }
        assert(q * b + r == a);
    }
    print("\n");
}

void test_div() {
    bigint a, b, c, d;
    a.nums = {0, 0, 0, 4};
    b.nums = {0, 2};
    c.nums = {0, 0, 2};
    d = a / b;
    assert(c == d);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_div();
    test_shift();

    test_add_commutative(10000);
    test_add_transitive(10000);
    test_add_sub_reverse(10000);
    test_add_sub_group(10000);
    test_mul_commutative(10000);
    test_mul_transitive(10000);
    test_mul_distributive(10000);
    test_div_perfect(10000);
    test_div_imperfect(10000);

    return 0;
}
