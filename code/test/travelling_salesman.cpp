#include "../travelling_salesman.hpp"

#include "test_utils.hpp"

// *****

// Datasets: https://people.sc.fsu.edu/~jburkardt/datasets/tsp/tsp.html (2020-08)

const string DATASET_FILE = "datasets/exact_tsp.txt";

bool trips_match(const vector<int>& a, vector<int> b) {
    if (a.size() != b.size() || a.empty() || a[0] != b[0])
        return false;
    if (a == b)
        return true;
    reverse(begin(b) + 1, end(b));
    return a == b;
}

struct exact_tsp_dataset_t {
    int V;
    vector<vector<int>> dist;
    int ans;
    vector<int> ans_path;

    void read(istream& in) {
        in >> V >> ans;
        assert(V > 0 && !in.bad());
        dist.assign(V, vector<int>(V)), ans_path.resize(V);

        for (int i = 0; i < V; i++) {
            in >> ans_path[i], ans_path[i]--;
        }
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                in >> dist[i][j];

        rotate(begin(ans_path), find(begin(ans_path), end(ans_path), 0), end(ans_path));
    }

    void run() const {
        vector<int> path;
        long optimum = tsp_held_karp(V, dist, &path);
        if (optimum != ans || !trips_match(path, ans_path)) {
            print(" exact tsp -- V={}\n", V);
            print("   expected: {:>9} | trip: {}\n", ans, fmt::join(ans_path, " "));
            print("     actual: {:>9} | trip: {}\n", optimum, fmt::join(path, " "));
        }
    }
};

void dataset_test_exact_tsp() {
    ifstream file(DATASET_FILE);
    if (!file.is_open())
        return print("dataset file {} not found, skipping dataset test\n", DATASET_FILE);
    file >> ws;
    while (!file.eof()) {
        exact_tsp_dataset_t test;
        test.read(file);
        test.run();
        file >> ws;
    }
    print_ok("dataset test exact tsp");
}

int main() {
    dataset_test_exact_tsp();
    return 0;
}
