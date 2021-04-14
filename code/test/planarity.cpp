#include "../graphs/left_right_planarity.hpp"
#include "test_utils.hpp"

// *****

const string DATASET_FILE = "datasets/planar.txt";

/**
 * Name
 * V E answer                   | answer := planar|nonplanar
 * u0 v0
 * u1 v1
 * ...
 */
struct planarity_dataset_test_t {
    string name, comment;
    edges_t g;
    int V, E;
    string ans;

    void read(istream& in) {
        getline(in, name);
        in >> V >> E >> ws >> ans >> ws;
        assert(V > 0 && E > 0 && !in.bad());
        assert(ans == "planar" || ans == "nonplanar");

        g.resize(E);
        for (auto& [u, v] : g)
            in >> u >> v;
    }

    void run() const {
        left_right lr(V, g);
        bool is_planar = lr.is_planar();
        string verdict = is_planar ? "planar" : "nonplanar";
        print("{:>9} | {:>9}  {}\n", verdict, ans, name);
    }
};

void dataset_test_planarity() {
    ifstream file(DATASET_FILE);
    if (!file.is_open())
        return print("dataset file {} not found, skipping dataset test\n", DATASET_FILE);
    file >> ws;
    print("===== planarity tests =====\n verdict  |  actual\n");
    while (!file.eof()) {
        planarity_dataset_test_t test;
        test.read(file);
        test.run();
        file >> ws;
    }
}

int main() {
    RUN_BLOCK(dataset_test_planarity());
    return 0;
}
