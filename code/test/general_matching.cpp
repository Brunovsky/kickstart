#include "../general_matching.hpp"

// *****

int N;
vector<string> names;
vector<micali_vazirani> graphs;
vector<int> maximum;

void read() {
    ifstream file("datasets/general_matching.txt");
    assert(file.is_open());
    file >> N >> ws;

    names.resize(N);
    maximum.resize(N);

    for (int i = 0; i < N; i++) {
        int V, E;
        getline(file, names[i]);
        file >> V >> E >> maximum[i] >> ws;

        micali_vazirani g(V);
        for (int e = 0; e < E; e++) {
            int u, v;
            file >> u >> v >> ws;
            g.add(u, v);
        }
        graphs.push_back(g);
    }
}

void test() {
    printf("===== GENERAL MATCHING TESTS =====\n answer | actual\n");

    for (int i = 0; i < N; i++) {
        auto& g = graphs[i];
        auto name = names[i].data();
        int card = g.max_matching();
        printf(" %6d | %6d   %4d%5d  %s\n", card, maximum[i], g.V, g.E, name);
    }
}

int main() {
    read();
    test();
    return 0;
}
