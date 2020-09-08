#include "../bigint.hpp"

#include "../debug_print.hpp"
#include "../random.hpp"

// *****

constexpr uint U = UINT_MAX;         // 0xffffffff
constexpr uint M = UINT_MAX / 2 + 1; // 0x80000000

reald distp(0.0, 1.0);
ulongd distv(0, U), distvp(1, U);
intd distn_small(0, 10), distn_pos(1, 8), distn_large(50, 300), distn_any(0, 40);
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

/**
 * test_add, test_sub, test_mul, test_div, test_mod, test_print
 * Unit tests for individual operations
 */
void test_add() {
    bigint u, v, a, b, c;

    for (int i = 0; i < 1'000'000; i++)
        u += 2 * i + 1, v -= 2 * i + 1;
    assert(u == bigint("1000000000000"));
    print("v: {}\no: {}\n", v, bigint("-1000000000000"));
    assert(v == bigint("-1000000000000"));

    u.nums = {M, U, U, U};
    u += M;
    a.nums = {0, 0, 0, 0, 1};
    assert(u == a);

    u.nums = {M, U, U - 1, U};
    u += M;
    a.nums = {0, 0, U, U};
    assert(u == a);

    a = "0123456789012345678901234567890123456789"s;
    b = "9876543210987654321098765432109876543210"s;
    c = "9999999999999999999999999999999999999999"s;
    u = a + b;
    assert(u == c);
    u -= b;
    assert(u == a);
}

void test_sub() {
    bigint u, a, b, c;

    vector<int> nums;
    for (int i = 0; i < 100000; i++)
        nums.push_back(!(i & 1) ? 1 + 2 * i : 1 - 2 * i);
    shuffle(begin(nums), end(nums), mt);
    for (int n : nums)
        u += n;
    assert(u == 0);
    shuffle(begin(nums), end(nums), mt);
    for (int n : nums)
        u -= n;
    assert(u == 0);

    u.nums = {7, 0, 0, 0, 1, 2};
    a.nums = {U, U, U, U, 0, 2};
    b = u;
    u -= 8;
    assert(u == a);
    b -= a;
    assert(b == 8);
}

void test_mul() {
    const bigint fac40("815915283247897734345611269596115894272000000000");
    bigint u = 1, v = 1, w = 1;
    for (int i = 1; i <= 40; i++)
        u *= i, v *= 2 * i, w *= 16 * i;
    assert(u == fac40);
    assert(v == fac40 << 40);
    assert(w == fac40 << 160);
    assert(v == w >> 120);
    assert(fac40 % 41 == 40);

    v = bigint("660955782884386677434829685779361532098606832525794499"
               "673096513026019562749349063704800410525656374299407003"
               "7769599882399012397170569200279466412758131334001");
    u = 1;
    for (int i = 1; i <= 100; i++)
        u *= 37;
    assert(u == v);
}

void test_div() {
    bigint a, b, c, d, x;
    a.nums = {0, 0, 0, 4};
    b.nums = {0, 2};
    c.nums = {0, 0, 2};
    d = a / b;
    assert(c == d);

    a = "123456789123456789123456789123456789"s;
    b = "987654321987654321"s;
    c = "124999998860937500"s;
    d = "137519289137519289"s;
    x = div_mod(a, b);
    assert(a == c && x == d);
}

void test_shift() {
    string s = "101011101100001101010101000001101100001111110101";
    string z = s + string(150, '0');
    bigint v(s, 2);
    int m = s.length();

    for (int i = 0; i < 128; i++) {
        bigint u = v << i;
        bigint w(z.substr(0, m + i), 2);
        assert(u == w);
    }
    for (int i = 0; i < 140; i++) {
        bigint u = (v << 127) >> i;
        bigint w(z.substr(0, m + (127 - i)), 2);
        assert(u == w);
    }
}

void test_print() {
    vector<bigint> ints = {
        bigint("123456789012345678901234567890"),
        bigint("12121212121212121212121212"),
        bigint("  -111222333444555666777888999000"),
        bigint("+123456789"),
        bigint("+987654321"),
        bigint("12345"),
        bigint("-54321"),
        bigint("123456789012345"),
        bigint("-987654321012345"),
        bigint("   -9999999999999999999999999999999999999"),
        bigint("1000000000000000000000000000000000000"),
    };
    for (int i = 0, n = ints.size(); i < n; i++)
        print("{:2}: {} {}\n", i, ints[i].len(), ints[i]);
}

void test_compare(int R) {
    vector<bigint> ints(R);
    for (int i = 0; i < R; i++) {
        ints[i] = random_bigint(distn_any(mt));
    }
    sort(begin(ints), end(ints));
    for (int i = 0; i + 1 < R; i++) {
        bigint dif = ints[i + 1] - ints[i];
        assert(dif.sign == 0 && dif >= 0);
    }
}

void test_add_commutative(int R) {
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

        if (q * b + r != a || !magnitude_cmp(r, b)) {
            print("\na: {}\nb: {}\nq: {}\nr: {}\n", a, b, q, r);
        }
        assert(q * b + r == a && magnitude_cmp(r, b));
    }
    print("\n");
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_add();
    test_sub();
    test_mul();
    test_div();
    test_shift();
    test_print();

    test_compare(10000);
    test_add_commutative(10000);
    test_add_transitive(10000);
    test_add_sub_reverse(10000);
    test_add_sub_group(10000);
    test_mul_commutative(10000);
    test_mul_transitive(10000);
    test_mul_distributive(10000);
    test_div_perfect(30000);
    test_div_imperfect(30000);

    return 0;
}
