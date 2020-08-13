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

    printf("graph hash: %lu\n", h1);
    for (int u = 0; u < 9; u++) {
        printf("hash[%d] = %lu\n", u, vhash1[u]);
    }
    fflush(stdout);
}

void test_generator() {
    auto g1 = generate_regular(15, 6);
    cout << g1 << endl;
    auto g2 = generate_regular(30, 4);
    cout << g2 << endl;
    auto g3 = generate_uniform(12, 0.2);
    cout << g3 << endl;
    auto g4 = generate_uniform_dag(13, 0.3);
    cout << g4 << endl;
    cout << reverse(g4) << endl;
    cout << relabel(g4) << endl;
    cout << join(g3, reverse(g4)) << endl;
    cout << generate_scc_uniform_expansion(g4, 5, 0.3) << endl;
}

void test_dot() {
    auto g1 = generate_uniform_dag(15, 0.2);
    ofstream file("datasets/dag.dot");
    assert(file.is_open());
    file << to_dot(g1, true);
}

int main() {
    test_isomorphic();
    test_generator();
    test_dot();
    return 0;
}
