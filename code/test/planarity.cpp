#include "test_utils.hpp"
#include "../graphs/left_right_planarity.hpp"

const string DATASET_FILE = "datasets/planar.txt";

inline namespace detail {

template <typename PT>
void add_edges(PT& planarity, const edges_t& g) {
    for (auto [u, v] : g) {
        planarity.add(u, v);
    }
}

} // namespace detail

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
        for (auto& [u, v] : g) {
            in >> u >> v;
        }
    }

    void run() const {
        // left_right lr(V);
        // add_edges(lr, g);
        // bool is_planar = lr.is_planar();
        bool is_planar = left_right_is_planar(V, g);
        string actual = is_planar ? "planar" : "nonplanar";
        print("{:>9} | {:>9} {}\n", ans, actual, name);
    }
};

void dataset_test_planarity() {
    ifstream file(DATASET_FILE);
    if (!file.is_open())
        return print("dataset file {} not found, skipping dataset test\n", DATASET_FILE);
    file >> ws;
    print("===== planarity tests =====\n expected  |  actual\n");
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
