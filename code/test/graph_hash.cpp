#include "../graph_hash.hpp"

// *****

// from wikipedia
void test_isomorphic() {
    int a = 2, b = 8, c = 3, d = 4, i = 1, j = 7, g = 6, h = 5;
    graph g1(9), g2(9);
    g1.add(1, 5), g1.add(2, 1), g1.add(1, 4);
    g1.add(6, 2), g1.add(5, 6), g1.add(6, 7);
    g1.add(4, 8), g1.add(8, 5), g1.add(7, 8);
    g1.add(2, 3), g1.add(4, 3), g1.add(3, 7);

    g2.add(a, g), g2.add(h, a), g2.add(a, i);
    g2.add(g, b), g2.add(b, h), g2.add(b, j);
    g2.add(c, g), g2.add(i, c), g2.add(j, c);
    g2.add(h, d), g2.add(i, d), g2.add(d, j);

    assert(isomorphic(g1, g2));
    auto vhash1 = hash_graph_vertices(g1);
    auto vhash2 = hash_graph_vertices(g2);
    auto h1 = hash_graph(g1);
    auto h2 = hash_graph(g2);
    assert(h1 == h2);
    assert(vhash1[1] == vhash2[a]);
    assert(vhash1[2] == vhash2[h]);
    assert(vhash1[3] == vhash2[d]);
    assert(vhash1[4] == vhash2[i]);
    assert(vhash1[5] == vhash2[g]);
    assert(vhash1[6] == vhash2[b]);
    assert(vhash1[7] == vhash2[j]);
    assert(vhash1[8] == vhash2[c]);
    assert(vhash1[0] == vhash2[0]);

    printf("graph hash: %20lu\n", h1);
    for (int u = 0; u < 9; u++) {
        printf("hash[%d] = %20lu\n", u, vhash1[u]);
    }
    printf("\n");
}

// tough case
void test_3regular() {
    auto add = [&](graph& g, int u, int v) {
        g.add(u, v);
        g.add(v, u);
    };

    graph g1(6), g2(6);
    add(g1, 0, 1), add(g1, 1, 2), add(g1, 2, 3);
    add(g1, 3, 4), add(g1, 4, 5), add(g1, 5, 0);
    add(g2, 0, 1), add(g2, 1, 2), add(g2, 2, 3);
    add(g2, 3, 4), add(g2, 4, 5), add(g2, 5, 0);

    add(g1, 1, 4), add(g1, 2, 5), add(g1, 3, 0);
    add(g2, 1, 5), add(g2, 2, 4), add(g2, 3, 0);

    assert(!isomorphic(g1, g2));
    auto vhash1 = hash_graph_vertices(g1);
    auto vhash2 = hash_graph_vertices(g2);
    auto h1 = hash_graph(g1);
    auto h2 = hash_graph(g2);
    printf("graph 1: %20lu\n", h1);
    printf("graph 2: %20lu\n", h2);
    for (int u = 0; u < 6; u++) {
        printf("hash[%d] = %20lu | %20lu\n", u, vhash1[u], vhash2[u]);
    }
    printf("\n");
    assert(h1 != h2);
    vhash1.erase(unique(begin(vhash1), end(vhash1)), end(vhash1));
    vhash2.erase(unique(begin(vhash2), end(vhash2)), end(vhash2));
    // these two graphs have undistinguishable nodes
    assert(vhash1.size() == 1U && vhash2.size() == 1U);
    assert(vhash1[0] != vhash2[0]);
}

int main() {
    test_isomorphic();
    test_3regular();
    return 0;
}
