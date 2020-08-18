#include "../graph_generator.hpp"

// *****

void test_generator() {
    auto g1 = generate_regular(15, 6);
    cout << "G1: Regular 15 V:\n" << g1 << endl;
    auto g2 = generate_regular(30, 4);
    cout << "G2: Regular 30 V:\n" << g2 << endl;
    auto g3 = generate_uniform_directed(12, 0.2);
    cout << "G3: Uniform directed 12 V:\n" << g3 << endl;
    auto g4 = generate_uniform_dag(13, 0.4);
    cout << "G4: Uniform dag 13 V:\n" << g4 << endl;
    cout << "Reverse G4:\n" << reverse(g4) << endl;
    cout << "Relabel G4:\n" << relabel(g4) << endl;
    cout << "Join G3 and G4:\n" << join(g3, reverse(g4)) << endl;
    auto g5 = generate_scc_uniform_expansion(g4, 5, 0.25);
    cout << "G5: Scc Expansion of G4 (expected 39 V):\n" << g5 << endl;
}

void test_dot() {
    auto g1 = generate_uniform_dag(15, 0.3);
    ofstream file("datasets/dag.dot");
    assert(file.is_open());
    file << to_dot(g1, true);
}

int main() {
    test_generator();
    // test_dot();
    return 0;
}
