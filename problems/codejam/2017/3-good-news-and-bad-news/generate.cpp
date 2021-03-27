#include "../../../../code/debug_print.hpp"
#include "../../../../code/hash.hpp"
#include "../../../../code/random.hpp"

// *****

auto int_gen(int lo, int hi) { return intd(lo, hi)(mt); }
auto real_gen(double lo, double hi) { return reald(lo, hi)(mt); }

constexpr int T = 100;

int main() {
    print("{}\n", T);
    for (int t = 1; t <= T; t++) {
        int V = int_gen(100, 200);
        int E = int_gen(2 * V, 3000);
        print("{} {}\n", V, E);
        unordered_set<array<int, 2>> edges;
        while (edges.size() < 0u + E) {
            int u = int_gen(1, V);
            int v = different(u, 1, V);
            edges.insert({u, v});
        }
        for (auto [u, v] : edges) {
            print("{} {}\n", u, v);
        }
    }
    return 0;
}
