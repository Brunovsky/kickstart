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
    printf("hash(%s): %zu\n", s1.data(), h1);
    printf("hash(%s): %zu\n", s2.data(), h2);
    printf("hash(%s): %zu\n", s3.data(), h3);
    size_t h2b = hasher.roll(h1, 'a', 'd');
    size_t h3b = hasher.roll(hasher.roll(h2b, 'a', 'e'), 'b', 'f');
    printf("roll(s2): %zu\n", h2b);
    printf("roll(s3): %zu\n", h3b);
    assert(h2 == h2b);
    assert(h3 == h3b);
    print_ok("unit test rolling hasher");
}

int main() {
    unit_test_rolling_hasher();
    return 0;
}
