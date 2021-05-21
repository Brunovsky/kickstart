#include "../../../../code/hacking.hpp"

// *****

int T = 100;

int main() {
    print("{}\n", T);
    for (int t = 1; t <= T; t++) {
        int V = randint(100, 200);
        int E = randint(2 * V, 3000);
        print("{} {}\n", V, E);
        unordered_set<array<int, 2>> edges;
        while (edges.size() < 0u + E) {
            int u = randint(1, V);
            int v = different(u, 1, V);
            edges.insert({u, v});
        }
        for (auto [u, v] : edges) {
            print("{} {}\n", u, v);
        }
    }
    return 0;
}
