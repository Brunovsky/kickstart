#include "test_utils.hpp"
#include "../graphs/isomorphism.hpp"
#include "../lib/graph_generator.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/isomorphism.hpp>

inline namespace detail {

using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;

bgraph make_boost(int V, const edges_t& g) {
    bgraph bg(V);
    for (auto [u, v] : g)
        add_edge(u, v, bg);
    return bg;
}

bool boost_test(int V, const edges_t& g1, const edges_t& g2) {
    auto bg1 = make_boost(V, g1);
    auto bg2 = make_boost(V, g2);
    return boost::isomorphism(bg1, bg2);
}

auto show(int negatives, int positives, int false_positives) {
    return format("{:5}n {:5}p {:5}fp ({:.1f}%)", negatives, positives, false_positives,
                  100.0 * false_positives / max(positives, 1));
}

auto generator_regular(int V, int kmin, int kmax) {
    return [=]() {
        int m = V % 2 ? 2 : 1;
        intd distk(kmin / m, kmax / m);
        int k = m * distk(mt);
        auto g1 = random_regular(V, k);
        auto g2 = random_regular(V, k);
        return tuple<int, edges_t, edges_t>{V, g1, g2};
    };
}

} // namespace detail

inline namespace stress_testing_isomorphism {

void stress_test_isomorphic_positives() {
    intd distV(10, 50);
    reald distp(0.1, 0.9);

    LOOP_FOR_DURATION_TRACKED(5s, now) {
        print_time(now, 5s, 50ms, "true test");

        int V = distV(mt);
        auto g1 = random_uniform_undirected_connected(V, distp(mt));
        auto g2 = relabel(V, g1);
        assert(isomorphic(V, g1, g2));
    }
}

template <typename Gn>
void fp_test_run(const string& name, Gn&& gn) {
    int negatives = 0, positives = 0, fps = 0;

    LOOP_FOR_DURATION_TRACKED_RUNS(4s, now, runs) {
        print_time(now, 4s, 50ms, show(negatives, positives, fps));

        auto [V, g1, g2] = gn();

        if (isomorphic(V, g1, g2)) {
            positives++;
            fps += !boost_test(V, g1, g2);
        } else {
            negatives++;
            assert(!boost_test(V, g1, g2));
        }
    }

    print_clear("{:>25} -- x{}  {}\n", name, runs, show(negatives, positives, fps));
}

void stress_test_isomorphic_false_positives() {
    print("graph isomorphism false positives test\n");
    fp_test_run("regular 11V k=2,4", generator_regular(11, 2, 4));
    fp_test_run("regular 11V k=6,8", generator_regular(11, 6, 8));
    fp_test_run("regular 12V k=3-5", generator_regular(12, 3, 5));
    fp_test_run("regular 12V k=6-8", generator_regular(12, 6, 8));
    for (int k = 10; k <= 11; k++) {
        fp_test_run("regular 14V k=" + to_string(k), generator_regular(14, k, k));
    }
}

} // namespace stress_testing_isomorphism

int main() {
    RUN_BLOCK(stress_test_isomorphic_positives());
    RUN_BLOCK(stress_test_isomorphic_false_positives());
    return 0;
}
