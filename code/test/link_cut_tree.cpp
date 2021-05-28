#include "test_utils.hpp"
#include "../lib/graph_formats.hpp"
#include "../lib/slow_tree.hpp"
#include "../struct/link_cut_tree_path.hpp"
#include "../struct/link_cut_tree_subtree.hpp"

#pragma GCC diagnostic ignored "-Wpedantic"
#pragma clang diagnostic ignored "-Wc99-designator"

inline namespace detail {

enum UnrootedActionType {
    LINK,     // pick two nodes in distinct trees and link them
    CUT,      // pick two connected nodes and unlink them
    LINK_CUT, // pick a node and link or cut

    FINDROOT,    // pick a node and call find root
    LCA,         // query lca between two random nodes
    LCA_CONN,    // query lca between two random connected nodes
    UPDATE_NODE, // update the value at a random node

    UPDATE_PATH, // update the path between two connected random nodes
    QUERY_PATH,  // query the path between two connected random nodes
    PATH_LENGTH, // query the path length between two connected random nodes

    UPDATE_SUBTREE, // update the subtree below a node with an outside node
    QUERY_SUBTREE,  // query subtree below a node with an outside node
    SUBTREE_SIZE,   // query subtree size below a node with an outside node

    STRESS_TEST, // query every fucking thing
    END,
};

string action_name[END + 1] = {
    "LINK",           "CUT",           "LINK_CUT",     "FINDROOT",    "LCA",
    "LCA CONN",       "UPDATE NODE",   "UPDATE PATH",  "QUERY PATH",  "PATH LENGTH",
    "UPDATE SUBTREE", "QUERY SUBTREE", "SUBTREE SIZE", "STRESS TEST", "END",
};

struct UnrootedAction {
    UnrootedActionType action = END;
    int u = -2, v = -2, r = -2, w = -2;
    long value;
    UnrootedAction(UnrootedActionType a) : action(a) {}
    UnrootedAction(UnrootedActionType a, int u, int v) : action(a), u(u), v(v) {}
    UnrootedAction(UnrootedActionType a, int u, int v, long value)
        : action(a), u(u), v(v), value(value) {}
    UnrootedAction(UnrootedActionType a, int u, int v, int r, int w)
        : action(a), u(u), v(v), r(r), w(w) {}
    UnrootedAction(UnrootedActionType a, int u, int v, int r, int w, long value)
        : action(a), u(u), v(v), r(r), w(w), value(value) {}
};

auto make_actions(int N, ms runtime, const int arr[END]) {
    constexpr long minv = 1, maxv = 50, mind = -30, maxd = 30;
    intd noded(1, N);
    longd vald(minv, maxv);
    longd deltad(mind, maxd);

    slow_tree slow(N);
    vector<int> occurrences(END);
    discrete_distribution<int> selector(arr, arr + END);

    vector<UnrootedAction> history;
    size_t Ssum = 0, iterations = 0;

    LOOP_FOR_DURATION_TRACKED(runtime, now) {
        print_time(now, runtime, 50ms, "preparing history (S={})...", slow.S);

        auto action = selector(mt);
        occurrences[action]++, iterations++;

        switch (action) {
        case LINK: {
            if (slow.S > 1) {
                auto [u, v] = slow.random_unconnected();
                slow.link(u, v);
                history.emplace_back(LINK, u, v);
            }
        } break;
        case CUT: {
            if (slow.S < N) {
                int u = slow.random_non_root();
                int v = slow.parent[u];
                assert(v != 0);
                slow.cut(u, v);
                history.emplace_back(CUT, u, v);
            }
        } break;
        case LINK_CUT: {
            int u = noded(mt);
            if (int v = slow.parent[u]; v) {
                slow.cut(u, v);
                history.emplace_back(CUT, u, v);
            } else if (slow.S > 1) {
                v = slow.random_not_in_subtree(u);
                slow.link(u, v);
                history.emplace_back(LINK, u, v);
            }
        } break;
        case FINDROOT: {
            int r = slow.random_root();
            int u = slow.random_in_subtree(r);
            history.emplace_back(FINDROOT, u, 0, r, 0);
        } break;
        case LCA: {
            int u = noded(mt);
            int v = noded(mt);
            int r = slow.random_in_tree(u);
            slow.reroot(r);
            int w = slow.lca(u, v);
            history.emplace_back(LCA, u, v, r, w);
        } break;
        case LCA_CONN: {
            if (slow.S < N) {
                auto [u, v] = slow.random_connected();
                int r = slow.random_in_tree(u);
                slow.reroot(r);
                int w = slow.lca(u, v);
                history.emplace_back(LCA, u, v, r, w);
            }
        } break;
        case UPDATE_NODE: {
            int u = noded(mt);
            auto val = vald(mt);
            slow.update_node(u, val);
            history.emplace_back(UPDATE_NODE, u, 0, val);
        } break;
        case UPDATE_PATH: {
            int u = noded(mt);
            int v = slow.random_in_tree(u);
            auto val = deltad(mt);
            slow.update_path(u, v, val);
            history.emplace_back(UPDATE_PATH, u, v, val);
        } break;
        case QUERY_PATH: {
            int u = noded(mt);
            int v = slow.random_in_tree(u);
            long ans = slow.query_path(u, v);
            history.emplace_back(QUERY_PATH, u, v, ans);
        } break;
        case PATH_LENGTH: {
            int u = noded(mt);
            int v = slow.random_in_tree(u);
            long ans = slow.path_length(u, v);
            history.emplace_back(PATH_LENGTH, u, v, ans);
        } break;
        case UPDATE_SUBTREE: {
            int u = noded(mt);
            int v = slow.random_in_tree(u);
            auto val = deltad(mt);
            slow.update_subtree(u, v, val);
            history.emplace_back(UPDATE_SUBTREE, u, v, val);
        } break;
        case QUERY_SUBTREE: {
            int u = noded(mt);
            int v = slow.random_in_tree(u);
            long ans = slow.query_subtree(u, v);
            history.emplace_back(QUERY_SUBTREE, u, v, ans);
        } break;
        case SUBTREE_SIZE: {
            int u = noded(mt);
            int v = slow.random_in_tree(u);
            long ans = slow.subtree_size(u, v);
            history.emplace_back(SUBTREE_SIZE, u, v, ans);
        } break;
        case STRESS_TEST: {
            history.emplace_back(STRESS_TEST);
        } break;
        default:
            break;
        }

        Ssum += slow.S;
    }

    print_clear("S avg: {:.2f}\n", 1.0 * Ssum / iterations);
    print("iterations: {}\n", iterations);
    // print("Frequency table:\n");
    // for (int a = 0; a < END; a++) {
    //     if (occurrences[a] > 0) {
    //         print("{:>15}: {}\n", action_name[a], occurrences[a]);
    //     }
    // }
    return history;
}

bool stress_verify_link_cut(slow_tree& slow, link_cut_tree_path& tree, int D = 2) {
    assert(1 <= D && D <= 9);
    int N = slow.N;

    vector<vector<int>> path(D), path_length(D);
    vector<int> exp_path(N + 1), exp_path_length(N + 1);
    vector<int> nodes(N), above(N + 1);
    iota(begin(nodes), end(nodes), 1);

    bool ok_path = true, ok_path_length = true;

    for (int u = 1; u <= N; u++) {
        above[u] = slow.random_in_tree(u);
        exp_path[u] = slow.query_path(u, above[u]);
        exp_path_length[u] = slow.path_length(u, above[u]);
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        path[d].resize(N + 1);
        for (int u : order) {
            path[d][u] = tree.query_path(u, above[u]);
        }
        if (path[d] != exp_path) {
            if (ok_path) {
                ok_path = false;
                print("      above: {}\n", above);
                print("expect_path: {}\n", exp_path);
            }
            print("got_path[{}]: {}\n", d + 1, path[d]);
        }
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        path_length[d].resize(N + 1);
        for (int u : order) {
            path_length[d][u] = tree.path_length(u, above[u]);
        }
        if (path_length[d] != exp_path_length) {
            if (ok_path_length) {
                ok_path_length = false;
                print("             above: {}\n", above);
                print("expect_path_length: {}\n", exp_path_length);
            }
            print("got_path_length[{}]: {}\n", d + 1, path_length[d]);
        }
    }

    return ok_path && ok_path_length;
}

bool stress_verify_link_cut(slow_tree& slow, link_cut_tree_subtree& tree, int D = 2) {
    assert(1 <= D && D <= 9);
    int N = slow.N;

    vector<vector<int>> subtree(D), subtree_size(D);
    vector<int> exp_subtree(N + 1), exp_subtree_size(N + 1);
    vector<int> nodes(N), above(N + 1);
    iota(begin(nodes), end(nodes), 1);

    bool ok_subtree = true, ok_subtree_size = true;

    for (int u = 1; u <= N; u++) {
        above[u] = slow.random_in_tree(u);
        exp_subtree[u] = slow.query_subtree(u, above[u]);
        exp_subtree_size[u] = slow.subtree_size(u, above[u]);
    }
    for (int d = 0; d < 0; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        subtree[d].resize(N + 1);
        for (int u : order) {
            subtree[d][u] = tree.query_subtree(u, above[u]);
        }
        if (subtree[d] != exp_subtree) {
            if (ok_subtree) {
                ok_subtree = false;
                print("expect_subtree: {}\n", exp_subtree);
            }
            print("got_subtree[{}]: {}\n", d + 1, subtree[d]);
        }
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        subtree_size[d].resize(N + 1);
        for (int u : order) {
            subtree_size[d][u] = tree.subtree_size(u, above[u]);
        }
        if (subtree_size[d] != exp_subtree_size) {
            if (ok_subtree_size) {
                ok_subtree_size = false;
                print("expect_subtree_size: {}\n", exp_subtree_size);
            }
            print("got_subtree_size[{}]: {}\n", d + 1, subtree_size[d]);
        }
    }

    return ok_subtree && ok_subtree_size;
}

} // namespace detail

inline namespace stress_testing {

constexpr int stress_path_ratio_arr[END] = {
    // topo 5000, lca 2500, update 5000, query 5000, stress 400
    [LINK] = 2250,       [CUT] = 750,          [LINK_CUT] = 2000,    [FINDROOT] = 1000,
    [LCA] = 500,         [LCA_CONN] = 1000,    [UPDATE_NODE] = 1500, [UPDATE_PATH] = 3500,
    [QUERY_PATH] = 3500, [PATH_LENGTH] = 1500, [UPDATE_SUBTREE] = 0, [QUERY_SUBTREE] = 0,
    [SUBTREE_SIZE] = 0,  [STRESS_TEST] = 400,
};

void stress_test_link_cut_tree_path(int N = 200, int T = 50'000) {
    slow_tree slow(N);
    link_cut_tree_path tree(N);
    auto actions = make_actions(N, 3s, stress_path_ratio_arr);
    deque<tuple<string, string>> states;
    int t = 0;
    bool ok = true;

    for (const auto& [action, u, v, r, w, val] : actions) {
        print_progress(t++, T, "stress test link cut tree");
        string label;

        switch (action) {
        case LINK: {
            slow.link(u, v);
            tree.link(u, v);
            label = format("[{}] LINK {}--{}", slow.S, u, v);
        } break;
        case CUT: {
            slow.cut(u, v);
            tree.cut(u, v);
            label = format("[{}] CUT {}--{}", slow.S, u, v);
        } break;
        case LCA: {
            tree.reroot(r);
            slow.reroot(r);
            int ans = tree.lca(u, v);
            ok = ans == w;
            label = format("[{}] LCA {}..{}, {}: {} / {}", slow.S, u, v, r, w, ans);
        } break;
        case FINDROOT: {
            tree.reroot(r);
            slow.reroot(r);
            int ans = tree.findroot(u);
            ok = ans == r;
            label = format("[{}] FINDROOT {}: {} / {}\n", slow.S, u, r, ans);
        } break;
        case UPDATE_NODE: {
            slow.update_node(u, val);
            tree.update_node(u, val);
            label = format("[{}] UPDATE {}: {}->{}", slow.S, u, slow.query_node(u), val);
        } break;
        case UPDATE_PATH: {
            slow.update_path(u, v, val);
            tree.update_path(u, v, val);
            label = format("[{}] UPDATE PATH {}..{}: {:+}", slow.S, u, v, val);
        } break;
        case QUERY_PATH: {
            long ans = tree.query_path(u, v);
            ok = val == ans;
            label = format("[{}] QUERY PATH {}..{}: {} / {}", slow.S, u, v, val, ans);
        } break;
        case PATH_LENGTH: {
            long ans = tree.path_length(u, v);
            ok = val == ans;
            label = format("[{}] PATH LENGTH {}..{}: {} / {}", slow.S, u, v, val, ans);
        } break;
        case STRESS_TEST: {
            ok = stress_verify_link_cut(slow, tree);
            label = format("[{}] STRESS TEST", slow.S);
        } break;
        default:
            print("Invalid action in actions\n");
            assert(false);
        }

        if (!ok) {
            print("{}\n", label);
            if (action != STRESS_TEST) {
                stress_verify_link_cut(slow, tree);
            }
            break;
        }
    }

    assert(ok);
}

constexpr int stress_subtree_ratio_arr[END] = {
    // topo 5000, lca 2500, update 5000, query 5000, stress 400
    [LINK] = 2250,         [CUT] = 750,          [LINK_CUT] = 2000,
    [FINDROOT] = 1000,     [LCA] = 500,          [LCA_CONN] = 1000,
    [UPDATE_NODE] = 5000,  [UPDATE_PATH] = 0,    [QUERY_PATH] = 0,
    [PATH_LENGTH] = 0,     [UPDATE_SUBTREE] = 0, [QUERY_SUBTREE] = 3500,
    [SUBTREE_SIZE] = 1500, [STRESS_TEST] = 400,
};

void stress_test_link_cut_tree_subtree(int N = 200, int T = 50'000) {
    slow_tree slow(N);
    link_cut_tree_subtree tree(N);
    auto actions = make_actions(N, 3s, stress_subtree_ratio_arr);
    deque<tuple<string, string>> states;
    int t = 0;
    bool ok = true;

    for (const auto& [action, u, v, r, w, val] : actions) {
        print_progress(t++, T, "stress test link cut tree subtree");
        string label;

        switch (action) {
        case LINK: {
            slow.link(u, v);
            tree.link(u, v);
            label = format("[{}] LINK {}--{}", slow.S, u, v);
        } break;
        case CUT: {
            slow.cut(u, v);
            tree.cut(u, v);
            label = format("[{}] CUT {}--{}", slow.S, u, v);
        } break;
        case LCA: {
            tree.reroot(r);
            int ans = tree.lca(u, v);
            label = format("[{}] LCA {}..{}, {}: {} / {}", slow.S, u, v, r, w, ans);
            ok = ans == w;
        } break;
        case FINDROOT: {
            tree.reroot(r);
            slow.reroot(r);
            int ans = tree.findroot(u);
            ok = ans == r;
            label = format("[{}] FINDROOT {}: {} / {}\n", slow.S, u, r, ans);
        } break;
        case UPDATE_NODE: {
            slow.update_node(u, val);
            tree.update_node(u, val);
            label = format("[{}] UPDATE {}: {}->{}", slow.S, u, slow.query_node(u), val);
        } break;
        // case UPDATE_SUBTREE: {
        //     slow.update_subtree(u, v, val);
        //     tree.update_subtree(u, v, val);
        //     label = format("[{}] UPDATE SUBTREE {}..{}: {:+}", slow.S, u, v, val);
        // } break;
        case QUERY_SUBTREE: {
            assert(u != 0 && v != 0);
            long ans = tree.query_subtree(u, v);
            ok = val == ans;
            label = format("[{}] QUERY SUBTREE {}\\{}: {} / {}", slow.S, u, v, val, ans);
        } break;
        case SUBTREE_SIZE: {
            assert(u != 0 && v != 0);
            long ans = tree.subtree_size(u, v);
            ok = val == ans;
            label = format("[{}] SUBTREE SIZE {}\\{}: {} / {}", slow.S, u, v, val, ans);
        } break;
        case STRESS_TEST: {
            label = format("[{}] STRESS TEST", slow.S);
            ok = stress_verify_link_cut(slow, tree);
        } break;
        default:
            print("Invalid action in actions\n");
            assert(false);
        }

        // states.push_back({label, tree.format_debug()});
        if (states.size() > 20u) {
            states.pop_front();
        }

        if (!ok) {
            print("{}\n", label);
            if (action != STRESS_TEST) {
                stress_verify_link_cut(slow, tree);
            }
            ofstream outdebug("debug.txt");
            for (auto [lbl, state] : states) {
                print(outdebug, "--- {}\n{}", lbl, state);
            }
            print("States output to debug.txt\n");
            break;
        }
    }

    assert(ok);
}

} // namespace stress_testing

inline namespace speed_testing {

constexpr int speed_topo_heavy[END] = {
    // topo 10000, update 5000, query 5000, lca 2000
    [LINK] = 4700,       [CUT] = 1300,         [LINK_CUT] = 4000,    [FINDROOT] = 1000,
    [LCA] = 800,         [LCA_CONN] = 1200,    [UPDATE_NODE] = 2500, [UPDATE_PATH] = 2500,
    [QUERY_PATH] = 3500, [PATH_LENGTH] = 1500, [UPDATE_SUBTREE] = 0, [QUERY_SUBTREE] = 0,
    [SUBTREE_SIZE] = 0,  [STRESS_TEST] = 0,
};
constexpr int speed_update_heavy[END] = {
    // topo 3000, update 14000, query 3000, lca 2000
    [LINK] = 1400,       [CUT] = 600,         [LINK_CUT] = 1000,    [FINDROOT] = 1000,
    [LCA] = 800,         [LCA_CONN] = 1200,   [UPDATE_NODE] = 6000, [UPDATE_PATH] = 8000,
    [QUERY_PATH] = 2400, [PATH_LENGTH] = 600, [UPDATE_SUBTREE] = 0, [QUERY_SUBTREE] = 0,
    [SUBTREE_SIZE] = 0,  [STRESS_TEST] = 0,
};
constexpr int speed_query_heavy[END] = {
    // topo 3000, update 3000, query 14000, lca 2000
    [LINK] = 1400,        [CUT] = 600,          [LINK_CUT] = 1000,
    [FINDROOT] = 1000,    [LCA] = 800,          [LCA_CONN] = 1200,
    [UPDATE_NODE] = 1200, [UPDATE_PATH] = 1800, [QUERY_PATH] = 10000,
    [PATH_LENGTH] = 4000, [UPDATE_SUBTREE] = 0, [QUERY_SUBTREE] = 0,
    [SUBTREE_SIZE] = 0,   [STRESS_TEST] = 0,
};

void speed_test_link_cut_tree_path(int N, const int arr[END]) {
    link_cut_tree_path tree(N);
    auto actions = make_actions(N, 4s, arr);

    START(linkcut);
    for (const auto& [action, u, v, r, w, val] : actions) {
        switch (action) {
        case LINK: {
            tree.link(u, v);
        } break;
        case CUT: {
            tree.cut(u, v);
        } break;
        case LCA: {
            tree.reroot(r);
            int ans = tree.lca(u, v);
            assert(ans == w);
        } break;
        case FINDROOT: {
            tree.reroot(r);
            int ans = tree.findroot(u);
            assert(ans == r);
        } break;
        case UPDATE_NODE: {
            tree.update_node(u, val);
        } break;
        case UPDATE_PATH: {
            tree.update_path(u, v, val);
        } break;
        case QUERY_PATH: {
            int ans = tree.query_path(u, v);
            assert(ans == val);
        } break;
        case PATH_LENGTH: {
            int ans = tree.path_length(u, v);
            assert(ans == val);
        } break;
        default:
            print("Invalid action in history\n");
            assert(false);
        }
    }
    TIME(linkcut);
    PRINT_TIME(linkcut);
}

} // namespace speed_testing

int main() {
    setbuf(stdout, nullptr);
    RUN_SHORT(stress_test_link_cut_tree_subtree());
    RUN_SHORT(stress_test_link_cut_tree_path());
    RUN_SHORT(speed_test_link_cut_tree_path(1'000, 70'000, speed_query_heavy));
    RUN_SHORT(speed_test_link_cut_tree_path(1'000, 70'000, speed_update_heavy));
    RUN_SHORT(speed_test_link_cut_tree_path(1'000, 70'000, speed_topo_heavy));
    return 0;
}
