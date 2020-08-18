#include "../planarity.hpp"

// *****

int N;
vector<string> names;
vector<left_right> graphs;
vector<string> planarity;

void read() {
    ifstream file("datasets/planar.txt");
    assert(file.is_open());
    file >> N >> ws;

    names.resize(N);
    planarity.resize(N);

    for (int i = 0; i < N; i++) {
        int V, E;
        getline(file, names[i]);
        file >> V >> E >> planarity[i] >> ws;

        left_right g(V);
        for (int e = 0; e < E; e++) {
            int u, v;
            file >> u >> v >> ws;
            g.add(u, v);
        }
        graphs.push_back(g);
    }
}

void test() {
    printf("===== PLANARITY TESTS =====\n verdict  |  actual\n");

    for (int i = 0; i < N; i++) {
        bool is_planar = graphs[i].is_planar();
        string verdict = is_planar ? "planar" : "nonplanar";
        printf("%9s | %9s  %s\n", verdict.data(), planarity[i].data(), names[i].data());
    }
}

int main() {
    read();
    test();
    return 0;
}
