#include "../dancing_links.hpp"

// *****

void test() {
    dancing_links_matrix dl(6, 6);

    assert(dl.north({4, 4}) == int2({3, 4}));
    assert(dl.south({4, 4}) == int2({5, 4}));
    assert(dl.west({4, 4}) == int2({4, 3}));
    assert(dl.west({5, 1}) == int2({5, 0}));
    assert(dl.east({4, 4}) == int2({4, 5}));
    assert(dl.east({2, 5}) == int2({2, 6}));
    dl.remove({3, 4});
    assert(dl.north({4, 4}) == int2({2, 4}));
}

int main() {
    test();
    return 0;
}
