#include "../graph_generator.hpp"

// *****

void test_generator() {
    auto g1 = generate_regular(10, 4);
    cout << "# G1: 4-Regular 10 V:\n" << g1 << endl;
    auto g2 = generate_regular(20, 5);
    cout << "# G2: 5-Regular 20 V:\n" << g2 << endl;

    auto g3 = generate_grid_directed(4, 5);
    cout << "# G3: Grid directed 4x5:\n" << g3 << endl;
    auto g4 = generate_grid_undirected(4, 5);
    cout << "# G4: Grid undirected 4x5:\n" << g4 << endl;

    auto g5 = generate_tree_directed(8);
    cout << "# G5: Tree directed 8 V:\n" << g5 << endl;
    auto g6 = generate_tree_undirected(9);
    cout << "# G6: Tree undirected 9 V:\n" << g6 << endl;

    auto g7 = generate_complete_directed(9);
    cout << "# G7: Complete directed 9 V:\n" << g7 << endl;
    auto g8 = generate_complete_undirected(8);
    cout << "# G8: Complete unirected 8 V:\n" << g8 << endl;

    auto g9 = generate_cycle_directed(6);
    cout << "# G9: Cycle directed 6 V:\n" << g9 << endl;
    auto g10 = generate_cycle_undirected(7);
    cout << "# G10: Cycle undirected 7 V:\n" << g10 << endl;

    auto g11 = generate_exact_rooted_dag(13, 63);
    cout << "# G11: Exact rooted dag 13 V, 63 E:\n" << g11 << endl;
    auto g12 = generate_exact_undirected(10, 23);
    cout << "# G12: Exact undirected 10 V, 23 E:\n" << g12 << endl;
}

void test_dot() {
    auto g1 = generate_uniform_rooted_dag(15, 0.3);
    ofstream file("datasets/dag.dot");
    assert(file.is_open());
    file << to_dot(g1, true);
}

int main() {
    test_generator();
    // test_dot();
    return 0;
}
