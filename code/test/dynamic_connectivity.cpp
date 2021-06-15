#include "test_utils.hpp"
#include "lib/event_time_tracker.hpp"
#include "../struct/dynamic_connectivity.hpp"
#include "../struct/pbds.hpp"
#include "../lib/slow_graph.hpp"

void unit_test_dynacon() {
    int S = 11;
    dynamic_connectivity dynacon(11);
    slow_graph slow(11);
    auto test_link = [&](int u, int v, bool ok) {
        bool is = dynacon.link(u, v);
        S -= is;
        print("link({:2},{:2}): {:5} {:5} {:5}\n", u, v, is, slow.link(u, v), ok);
        assert(S == slow.num_components());
    };
    auto test_cut = [&](int u, int v, bool ok) {
        bool is = dynacon.cut(u, v);
        S += is;
        print(" cut({:2},{:2}): {:5} {:5} {:5}\n", u, v, is, slow.cut(u, v), ok);
        assert(S == slow.num_components());
    };
    [[maybe_unused]] auto test_conn = [&](int u, int v, bool ok) {
        bool is = dynacon.link(u, v);
        print("conn({:2},{:2}): {:5} {:5} {:5}\n", u, v, is, slow.conn(u, v), ok);
        assert(S == slow.num_components());
    };
    test_link(3, 8, true);
    test_link(7, 2, true);
    test_link(9, 7, true);
    test_link(1, 5, true);
    test_link(10, 1, true);
    test_link(2, 9, false);
    test_link(1, 11, true);
    test_link(10, 11, false);
    test_link(1, 3, true);
    test_link(5, 8, false);
    test_cut(1, 5, false);
    test_link(5, 6, true);
    test_link(1, 6, false);
    test_link(6, 11, false);
    test_cut(1, 11, false);
    test_link(2, 1, true);
    test_cut(7, 9, false);
    test_cut(3, 8, false);
    test_conn(2, 1, true);
    test_conn(10, 1, true);
}

void random_test_dynacon() {
    enum Action { LINK, CUT, LINK_CUT };
    vector<string> event_names = {"LINK", "CUT", "LINK_CUT"};
    boold coind(0.5);

    event_time_tracker tracker({1000, 1990, 1000});

    constexpr int N = 3000;
    dynamic_connectivity dynacon(N);
    slow_graph slow(N);
    int S = N, E = 0;
    ordered_set<pair<int, int>> edges;

    auto has_edge = [&](int u, int v) { return edges.find(minmax(u, v)) != edges.end(); };
    auto add_edge = [&](int u, int v) {
        if (coind(mt))
            swap(u, v);

        edges.insert(minmax(u, v)), E++;
        slow.link(u, v);

        tracker.start_event(LINK);
        S -= dynacon.link(u, v);
        tracker.time();
    };
    auto rem_edge = [&](int u, int v) {
        if (coind(mt))
            swap(u, v);

        edges.erase(minmax(u, v)), E--;
        slow.cut(u, v);

        tracker.start_event(CUT);
        S += dynacon.cut(u, v);
        tracker.time();
    };

    // Start off with random edge additions
    for (int i = 0; i < N / 3; i++) {
        print_regular(i, N, 100, "initial edges... S,E={}", S, E);
        auto [u, v] = different(1, N + 1);
        if (!has_edge(u, v)) {
            add_edge(u, v);
        }
    }

    assert(S == slow.num_components());
    deque<string> labels;

    LOOP_FOR_DURATION_TRACKED_RUNS (30s, now, runs) {
        print_time(now, 30s, "test dynacon ({} runs, S,E={},{})", runs, S, E);

        int event = tracker.next_event();
        string label;

        switch (event) {
        case LINK: {
            auto [u, v] = different(1, N + 1);
            if (!has_edge(u, v)) {
                add_edge(u, v);
                label = format("[{}] LINK {}--{}", slow.num_components(), u, v);
            }
        } break;
        case CUT: {
            if (E > 0) {
                auto [u, v] = *edges.find_by_order(intd(0, E - 1)(mt));
                rem_edge(u, v);
                label = format("[{}] CUT {}--{}", slow.num_components(), u, v);
            }
        } break;
        case LINK_CUT: {
            auto [u, v] = different(1, N + 1);
            if (coind(mt))
                swap(u, v);
            if (has_edge(u, v)) {
                rem_edge(u, v);
                label = format("[{}] CUT {}--{}", slow.num_components(), u, v);
            } else {
                add_edge(u, v);
                label = format("[{}] LINK {}--{}", slow.num_components(), u, v);
            }
        } break;
        }

        if (!label.empty()) {
            labels.push_back(label);
        }
        if (labels.size() > 20u) {
            labels.pop_front();
        }
        if (S != slow.num_components()) {
            printcl("{}", fmt::join(labels, "{}\n"));
        }
        assert(S == slow.num_components());
    }

    tracker.pretty_log(event_names);
}

int main() {
    RUN_SHORT(unit_test_dynacon());
    RUN_BLOCK(random_test_dynacon());
    return 0;
}
