#include "../travelling_salesman.hpp"

// *****

// Datasets: https://people.sc.fsu.edu/~jburkardt/datasets/tsp/tsp.html (2020-08)

int N;
vector<travelling_salesman> tsps;
vector<int> optimums;
vector<vector<int>> trips;

void read_dataset() {
    ifstream fin("datasets/exact_tsp.txt");
    assert(fin.is_open());
    fin >> N;
    tsps.resize(N), optimums.resize(N), trips.assign(N, {});
    for (int n = 0; n < N; n++) {
        int V;
        travelling_salesman tsp;
        fin >> V;
        tsp.V = V;
        tsp.dist.resize(V, vector<int>(V));
        vector<int> trip(V);
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                fin >> tsp.dist[i][j];
            }
        }
        fin >> optimums[n];
        for (int i = 0; i < V; i++) {
            fin >> trip[i];
            trip[i]--;
        }
        rotate(begin(trip), find(begin(trip), end(trip), 0), end(trip));
        tsps[n] = tsp;
        trips[n] = trip;
        assert(!fin.bad());
    }
}

void print_tsp(int i, const pair<int, vector<int>>& trip) {
    printf("optimum #%d: %d\n", i, trip.first);
    printf("trip:");
    for (int v : trip.second)
        printf(" %d", v);
    printf("\n");
}

bool trips_match(vector<int> a, vector<int> b) {
    assert(!a.empty() && a.size() == b.size() && a[0] == b[0]);
    if (a == b)
        return true;
    reverse(begin(b) + 1, end(b));
    return a == b;
}

void test() {
    for (int i = 0; i < N; i++) {
        auto res = tsps[i].compute();
        print_tsp(i + 1, res);
        assert(res.first == optimums[i] && trips_match(res.second, trips[i]));
    }
}

int main() {
    read_dataset();
    test();
    return 0;
}
