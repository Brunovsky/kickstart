#include "../lib/graph_formats.hpp"
#include "../struct/unrooted_link_cut_tree.hpp"
#include "test_utils.hpp"
#include "../lib/slow_tree.hpp"

#pragma GCC diagnostic ignored "-Wpedantic"

enum UnrootedActionType {
    LINK,        // pick two nodes in distinct trees and link them
    CUT,         // pick two connected nodes and unlink them
    LINK_CUT,    // pick a node and link or cut
    LCA,         // query lca between two random nodes
    LCA_CONN,    // query lca between two random connected nodes
    UPDATE_NODE, // update the value at a random node
    UPDATE_PATH, // update the path between two connected random nodes
    STRESS_TEST, // query every fucking thing
    QUERY_PATH,  // query the path between two connected random nodes
    PATH_LENGTH, // query the path length between two connected random nodes
    END,
};
constexpr int stress_ratio_arr[END] = {
    // topo 2000, update 3000, query 2000, lca 1000, stress 200
    [LINK] = 750,        [CUT] = 250,          [LINK_CUT] = 1000,    [LCA] = 400,
    [LCA_CONN] = 600,    [UPDATE_NODE] = 1000, [UPDATE_PATH] = 2000, [STRESS_TEST] = 200,
    [QUERY_PATH] = 1200, [PATH_LENGTH] = 800,
};
constexpr int speed_topo_heavy[END] = {
    // topo 10000, update 5000, query 5000, lca 2000
    [LINK] = 4700,       [CUT] = 1300,         [LINK_CUT] = 4000,    [LCA] = 800,
    [LCA_CONN] = 1200,   [UPDATE_NODE] = 2500, [UPDATE_PATH] = 2500, [STRESS_TEST] = 0,
    [QUERY_PATH] = 3500, [PATH_LENGTH] = 1500,
};
constexpr int speed_update_heavy[END] = {
    // topo 3000, update 14000, query 3000, lca 2000
    [LINK] = 1400,       [CUT] = 600,          [LINK_CUT] = 1000,    [LCA] = 800,
    [LCA_CONN] = 1200,   [UPDATE_NODE] = 6000, [UPDATE_PATH] = 8000, [STRESS_TEST] = 0,
    [QUERY_PATH] = 2400, [PATH_LENGTH] = 600,
};
constexpr int speed_query_heavy[END] = {
    // topo 3000, update 3000, query 14000, lca 2000
    [LINK] = 1400,        [CUT] = 600,          [LINK_CUT] = 1000,    [LCA] = 800,
    [LCA_CONN] = 1200,    [UPDATE_NODE] = 1200, [UPDATE_PATH] = 1800, [STRESS_TEST] = 0,
    [QUERY_PATH] = 10000, [PATH_LENGTH] = 4000,
};

struct UnrootedAction {
    UnrootedActionType action;
    int u, v, r, w;
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

auto make_actions(int N, int T, const int arr[END]) {
    constexpr long minv = 1, maxv = 50, mind = -30, maxd = 30;
    intd noded(1, N);
    longd vald(minv, maxv);
    longd deltad(mind, maxd);

    slow_tree slow(N, true);
    vector<int> ratio(arr, arr + END);
    vector<int> occurrences(END);
    action_selector<UnrootedActionType> selector(ratio);

    vector<UnrootedAction> history;
    size_t Ssum = 0, iterations = 0;

    while (int(history.size()) < T) {
        print_progress(history.size(), T, format("preparing history (S={})...", slow.S));
        auto action = selector.select();
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
                slow.cut(u, v);
                history.emplace_back(CUT, u, v);
            }
        } break;
        case LINK_CUT: {
            int u = noded(mt);
            if (int v = slow.parent[u]; v) {
                slow.cut(u, v);
                history.emplace_back(CUT, u, v);
            } else if (slow.S < N) {
                v = slow.random_not_in_subtree(u);
                slow.link(u, v);
                history.emplace_back(LINK, u, v);
            }
        } break;
        case LCA: {
            int u = noded(mt);
            int v = noded(mt);
            int r = slow.random_in_subtree(u);
            slow.reroot(r);
            int w = slow.lca(u, v);
            history.emplace_back(LCA, u, v, r, w);
        } break;
        case LCA_CONN: {
            if (slow.S < N) {
                auto [u, v] = slow.random_connected();
                int r = slow.random_in_subtree(u);
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
        case STRESS_TEST: {
            history.emplace_back(STRESS_TEST);
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
        default:
            break;
        }

        Ssum += slow.S;
    }

    clear_line();
    print("S avg: {:.2f}\n", 1.0 * Ssum / iterations);
    return history;
}

bool stress_verify_unrooted_link_cut(slow_tree& slow, unrooted_link_cut_tree& tree,
                                     int D = 2) {
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

void stress_test_unrooted_link_cut_tree(int N = 100, int T = 25'000) {
    slow_tree slow(N, true);
    unrooted_link_cut_tree tree(N);
    auto actions = make_actions(N, T, stress_ratio_arr);
    deque<tuple<int, string, string, string>> states;
    int t = 0;
    bool ok = true;

    for (const auto& [action, u, v, r, w, val] : actions) {
        print_progress(t++, T, "stress test unrooted link cut tree");
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
        case STRESS_TEST: {
            label = format("[{}] STRESS TEST", slow.S);
            ok = stress_verify_unrooted_link_cut(slow, tree);
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
        default:
            print("Invalid action in actions\n");
            assert(false);
        }

        if (!ok) {
            break;
        }
    }

    assert(ok);
}

void speed_test_unrooted_link_cut_tree(int N, int T, const int arr[END]) {
    unrooted_link_cut_tree tree(N);
    auto actions = make_actions(N, T, arr);

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

int main() {
    setbuf(stdout, nullptr);
    RUN_SHORT(stress_test_unrooted_link_cut_tree());
    RUN_SHORT(speed_test_unrooted_link_cut_tree(1'000, 70'000, speed_query_heavy));
    RUN_SHORT(speed_test_unrooted_link_cut_tree(1'000, 70'000, speed_update_heavy));
    RUN_SHORT(speed_test_unrooted_link_cut_tree(1'000, 70'000, speed_topo_heavy));
    return 0;
}
