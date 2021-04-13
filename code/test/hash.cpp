#include "../hash.hpp"

#include "test_utils.hpp"

// *****

void unit_test_rolling_hasher() {
    rolling_hasher hasher(10);
    string s1 = "aababbcacc";
    string s2 = "ababbcaccd";
    string s3 = "abbcaccdef";
    size_t h1 = hasher(s1);
    size_t h2 = hasher(s2);
    size_t h3 = hasher(s3);
    print("hash({}): {}\n", s1.data(), h1);
    print("hash({}): {}\n", s2.data(), h2);
    print("hash({}): {}\n", s3.data(), h3);
    size_t h2b = hasher.roll(h1, 'a', 'd');
    size_t h3b = hasher.roll(hasher.roll(h2b, 'a', 'e'), 'b', 'f');
    print("roll(s2): {}\n", h2b);
    print("roll(s3): {}\n", h3b);
    assert(h2 == h2b);
    assert(h3 == h3b);
    print_ok("unit test rolling hasher");
}

void unit_test_tuple_hasher() {
    Hasher hasher;
    print("hash<3, 'abc', 7L> = {}\n", hasher(make_tuple(3, "abc"s, 7L)));
    print("hash<2, 'abc', 7L> = {}\n", hasher(make_tuple(2, "abc"s, 7L)));
    print("hash<1, 0, 0> = {}\n", hasher(make_tuple(1, 0, 0)));
    print("hash<0, 1, 0> = {}\n", hasher(make_tuple(0, 1, 0)));
    print("hash<0, 0, 1> = {}\n", hasher(make_tuple(0, 0, 1)));
    print("hash<2, 0, 0> = {}\n", hasher(make_tuple(2, 0, 0)));
    print("hash<0, 2, 0> = {}\n", hasher(make_tuple(0, 2, 0)));
    print("hash<0, 0, 2> = {}\n", hasher(make_tuple(0, 0, 2)));
}

int main() {
    unit_test_rolling_hasher();
    unit_test_tuple_hasher();
    return 0;
}
