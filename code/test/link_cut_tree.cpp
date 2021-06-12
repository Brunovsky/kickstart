#include "test_utils.hpp"
#include "../lib/graph_formats.hpp"
#include "../lib/slow_tree.hpp"
#include "../struct/link_cut_tree_path.hpp"
#include "../struct/link_cut_tree_subtree.hpp"
#include "lib/tree_action.hpp"

using lct_path = link_cut_tree_path<lct_node_path_sum>;
using lct_subtree = link_cut_tree_subtree<lct_node_subtree_sum>;
using namespace tree_testing;

inline namespace detail {

bool stress_verify_link_cut(slow_tree& slow, lct_path& tree, int D = 2) {
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
            path[d][u] = tree.access_path(u, above[u])->path;
        }
        if (path[d] != exp_path) {
            if (ok_path) {
                ok_path = false;
                printcl("      above: {}\n", above);
                printcl("expect_path: {}\n", exp_path);
            }
            printcl("got_path[{}]: {}\n", d + 1, path[d]);
        }
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        path_length[d].resize(N + 1);
        for (int u : order) {
            path_length[d][u] = tree.access_path(u, above[u])->path_size;
        }
        if (path_length[d] != exp_path_length) {
            if (ok_path_length) {
                ok_path_length = false;
                printcl("             above: {}\n", above);
                printcl("expect_path_length: {}\n", exp_path_length);
            }
            printcl("got_path_length[{}]: {}\n", d + 1, path_length[d]);
        }
    }

    return ok_path && ok_path_length;
}

bool stress_verify_link_cut(slow_tree& slow, lct_subtree& tree, int D = 2) {
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
            subtree[d][u] = tree.access_subtree(u, above[u])->subtree();
        }
        if (subtree[d] != exp_subtree) {
            if (ok_subtree) {
                ok_subtree = false;
                printcl("expect_subtree: {}\n", exp_subtree);
            }
            printcl("got_subtree[{}]: {}\n", d + 1, subtree[d]);
        }
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        subtree_size[d].resize(N + 1);
        for (int u : order) {
            subtree_size[d][u] = tree.access_subtree(u, above[u])->subtree_size();
        }
        if (subtree_size[d] != exp_subtree_size) {
            if (ok_subtree_size) {
                ok_subtree_size = false;
                printcl("expect_subtree_size: {}\n", exp_subtree_size);
            }
            printcl("got_subtree_size[{}]: {}\n", d + 1, subtree_size[d]);
        }
    }

    return ok_subtree && ok_subtree_size;
}

} // namespace detail

inline namespace stress_testing {

actions_t<UnrootedAT> stress_path_ratio = {
    {UnrootedAT::LINK, 1500},        {UnrootedAT::CUT, 300},
    {UnrootedAT::LINK_CUT, 1200},    {UnrootedAT::FINDROOT, 1000},
    {UnrootedAT::LCA, 500},          {UnrootedAT::LCA_CONN, 1000},
    {UnrootedAT::QUERY_NODE, 2000},  {UnrootedAT::UPDATE_NODE, 1500},
    {UnrootedAT::UPDATE_PATH, 3500}, {UnrootedAT::QUERY_PATH, 3500},
    {UnrootedAT::PATH_LENGTH, 1500}, {UnrootedAT::STRESS_TEST, 400},
};

void stress_test_link_cut_tree_path(int N = 200) {
    slow_tree slow(N, false);
    lct_path tree(N);
    auto actions = make_unrooted_actions(N, 400ms, stress_path_ratio, N - 100);
    bool ok = true;

    for (int ia = 0, A = actions.size(); ia < A; ia++) {
        print_regular(ia, A, 1000, "stress test lct subtree");
        auto [action, u, v, r, who, val] = actions[ia];
        string label;

        switch (action) {
        case UnrootedAT::LINK: {
            slow.link(u, v);
            bool fine = tree.link(u, v);
            assert(fine);
            label = format("[{}] LINK {}--{}", slow.S, u, v);
        } break;
        case UnrootedAT::CUT: {
            slow.cut(u, v);
            bool fine = tree.cut(u, v);
            assert(fine);
            label = format("[{}] CUT {}--{}", slow.S, u, v);
        } break;
        case UnrootedAT::FINDROOT: {
            tree.reroot(who);
            slow.reroot(who);
            int ans = tree.findroot(u);
            ok = ans == who;
            label = format("[{}] FINDROOT {}: ={} ?{}\n", slow.S, u, who, ans);
        } break;
        case UnrootedAT::LCA: {
            tree.reroot(r);
            slow.reroot(r);
            int ans = tree.lca(u, v);
            ok = ans == who;
            label = format("[{}] LCA {}..{}, {}: ={} ?{}", slow.S, u, v, r, who, ans);
        } break;
        case UnrootedAT::QUERY_NODE: {
            long ans = tree.access_node(u)->self;
            ok = val == ans;
            label = format("[{}] QUERY {}: ={} ?{}", slow.S, u, val, ans);
        } break;
        case UnrootedAT::UPDATE_NODE: {
            long init = slow.query_node(u);
            slow.update_node(u, val);
            tree.access_node(u)->self = val;
            label = format("[{}] UPDATE {}: {}->{}", slow.S, u, init, val);
        } break;
        case UnrootedAT::QUERY_PATH: {
            long ans = tree.access_path(u, v)->path;
            ok = val == ans;
            label = format("[{}] QUERY PATH {}..{}: ={} ?{}", slow.S, u, v, val, ans);
        } break;
        case UnrootedAT::PATH_LENGTH: {
            long ans = tree.access_path(u, v)->path_size;
            ok = val == ans;
            label = format("[{}] PATH LENGTH {}..{}: ={} ?{}", slow.S, u, v, val, ans);
        } break;
        case UnrootedAT::UPDATE_PATH: {
            slow.update_path(u, v, val);
            tree.access_path(u, v)->lazy += val;
            label = format("[{}] UPDATE PATH {}..{}: {:+}", slow.S, u, v, val);
        } break;
        case UnrootedAT::STRESS_TEST: {
            ok = stress_verify_link_cut(slow, tree);
            label = format("[{}] STRESS TEST", slow.S);
        } break;
        default:
            throw runtime_error("Unsupported action");
        }
        if (!ok) {
            printcl("Failed action: {}\n", action_names<UnrootedAT>.at(action));
        }
        assert(ok);
    }

    assert(ok);
}

actions_t<UnrootedAT> stress_subtree_ratio = {
    {UnrootedAT::LINK, 1500},         {UnrootedAT::CUT, 300},
    {UnrootedAT::LINK_CUT, 1200},     {UnrootedAT::FINDROOT, 1000},
    {UnrootedAT::LCA, 500},           {UnrootedAT::LCA_CONN, 1000},
    {UnrootedAT::QUERY_NODE, 2000},   {UnrootedAT::UPDATE_NODE, 5000},
    {UnrootedAT::UPDATE_SUBTREE, 0},  {UnrootedAT::QUERY_SUBTREE, 3500},
    {UnrootedAT::SUBTREE_SIZE, 1500}, {UnrootedAT::STRESS_TEST, 400},
};

void stress_test_lct_subtree(int N = 200) {
    slow_tree slow(N, false);
    lct_subtree tree(N);
    auto actions = make_unrooted_actions(N, 400ms, stress_subtree_ratio, 9 * N / 10);
    bool ok = true;

    for (int ia = 0, A = actions.size(); ia < A; ia++) {
        print_regular(ia, A, 1000, "stress test lct subtree");
        auto [action, u, v, r, who, val] = actions[ia];
        string label;

        switch (action) {
        case UnrootedAT::LINK: {
            slow.link(u, v);
            bool fine = tree.link(u, v);
            assert(fine);
            label = format("[{}] LINK {}--{}", slow.S, u, v);
        } break;
        case UnrootedAT::CUT: {
            slow.cut(u, v);
            bool fine = tree.cut(u, v);
            assert(fine);
            label = format("[{}] CUT {}--{}", slow.S, u, v);
        } break;
        case UnrootedAT::FINDROOT: {
            tree.reroot(who);
            slow.reroot(who);
            int ans = tree.findroot(u);
            ok = ans == who;
            label = format("[{}] FINDROOT {}: ={} ?{}\n", slow.S, u, who, ans);
        } break;
        case UnrootedAT::LCA: {
            tree.reroot(r);
            slow.reroot(r);
            int ans = tree.lca(u, v);
            ok = ans == who;
            label = format("[{}] LCA {}..{}, {}: ={} ?{}", slow.S, u, v, r, who, ans);
        } break;
        case UnrootedAT::QUERY_NODE: {
            long ans = tree.access_node(u)->self;
            ok = val == ans;
            label = format("[{}] QUERY {}: ={} ?{}", slow.S, u, val, ans);
        } break;
        case UnrootedAT::UPDATE_NODE: {
            long init = slow.query_node(u);
            slow.update_node(u, val);
            tree.access_node(u)->self = val;
            label = format("[{}] UPDATE {}: {}->{}", slow.S, u, init, val);
        } break;
        case UnrootedAT::QUERY_SUBTREE: {
            long ans = tree.access_subtree(u, v)->subtree();
            ok = val == ans;
            label = format("[{}] QUERY SUBTREE {}..{}: ={} ?{}", slow.S, u, v, val, ans);
        } break;
        case UnrootedAT::SUBTREE_SIZE: {
            long ans = tree.access_subtree(u, v)->subtree_size();
            ok = val == ans;
            label = format("[{}] SUBTREE SIZE {}..{}: ={} ?{}", slow.S, u, v, val, ans);
        } break;
        case UnrootedAT::STRESS_TEST: {
            ok = stress_verify_link_cut(slow, tree);
            label = format("[{}] STRESS TEST", slow.S);
        } break;
        default:
            throw runtime_error("Unsupported action");
        }
        if (!ok) {
            printcl("Failed action: {}\n", action_names<UnrootedAT>.at(action));
        }
        assert(ok);
    }

    assert(ok);
}

} // namespace stress_testing

inline namespace speed_testing_lct_path {

actions_t<UnrootedAT> path_speed_topo_heavy_actions = {
    {UnrootedAT::LINK, 5000},        {UnrootedAT::CUT, 1000},
    {UnrootedAT::LINK_CUT, 4000},    {UnrootedAT::FINDROOT, 1000},
    {UnrootedAT::LCA, 800},          {UnrootedAT::LCA_CONN, 1200},
    {UnrootedAT::QUERY_NODE, 2000},  {UnrootedAT::UPDATE_NODE, 2500},
    {UnrootedAT::UPDATE_PATH, 2500}, {UnrootedAT::QUERY_PATH, 3500},
    {UnrootedAT::PATH_LENGTH, 1500},
};
actions_t<UnrootedAT> path_speed_update_heavy_actions = {
    {UnrootedAT::LINK, 1500},        {UnrootedAT::CUT, 300},
    {UnrootedAT::LINK_CUT, 1200},    {UnrootedAT::FINDROOT, 1000},
    {UnrootedAT::LCA, 800},          {UnrootedAT::LCA_CONN, 1200},
    {UnrootedAT::QUERY_NODE, 1000},  {UnrootedAT::UPDATE_NODE, 6000},
    {UnrootedAT::UPDATE_PATH, 8000}, {UnrootedAT::QUERY_PATH, 2400},
    {UnrootedAT::PATH_LENGTH, 600},
};
actions_t<UnrootedAT> path_speed_query_heavy_actions = {
    {UnrootedAT::LINK, 1500},        {UnrootedAT::CUT, 300},
    {UnrootedAT::LINK_CUT, 1200},    {UnrootedAT::FINDROOT, 1000},
    {UnrootedAT::LCA, 800},          {UnrootedAT::LCA_CONN, 1200},
    {UnrootedAT::QUERY_NODE, 5000},  {UnrootedAT::UPDATE_NODE, 1200},
    {UnrootedAT::UPDATE_PATH, 1800}, {UnrootedAT::QUERY_PATH, 10000},
    {UnrootedAT::PATH_LENGTH, 4000},
};

void speed_test_lct_path(int N, const actions_t<UnrootedAT>& freq) {
    lct_path tree(N);
    auto actions = make_unrooted_actions(N, 10s, freq, N - 100);

    START(linkcut);
    for (const auto& [action, u, v, r, who, val] : actions) {
        bool ok = true;
        switch (action) {
        case UnrootedAT::LINK: {
            tree.link(u, v);
        } break;
        case UnrootedAT::CUT: {
            tree.cut(u, v);
        } break;
        case UnrootedAT::FINDROOT: {
            tree.reroot(who);
            int ans = tree.findroot(u);
            ok = ans == who;
        } break;
        case UnrootedAT::LCA: {
            tree.reroot(r);
            int ans = tree.lca(u, v);
            ok = ans == who;
        } break;
        case UnrootedAT::QUERY_NODE: {
            long ans = tree.access_node(u)->self;
            ok = val == ans;
        } break;
        case UnrootedAT::UPDATE_NODE: {
            tree.access_node(u)->self = val;
        } break;
        case UnrootedAT::QUERY_PATH: {
            long ans = tree.access_path(u, v)->path;
            ok = val == ans;
        } break;
        case UnrootedAT::PATH_LENGTH: {
            long ans = tree.access_path(u, v)->path_size;
            ok = val == ans;
        } break;
        case UnrootedAT::UPDATE_PATH: {
            tree.access_path(u, v)->lazy += val;
        } break;
        default:
            throw runtime_error("Unsupported action");
        }
        if (!ok) {
            printcl("Failed action: {}\n", action_names<UnrootedAT>.at(action));
        }
        assert(ok);
    }
    TIME(linkcut);
    PRINT_EACH_NS(linkcut, actions.size());
}

} // namespace speed_testing_lct_path

inline namespace speed_testing_lct_subtree {

actions_t<UnrootedAT> subtree_speed_topo_heavy_actions = {
    {UnrootedAT::LINK, 5000},         {UnrootedAT::CUT, 1000},
    {UnrootedAT::LINK_CUT, 4000},     {UnrootedAT::FINDROOT, 1000},
    {UnrootedAT::LCA, 800},           {UnrootedAT::LCA_CONN, 1200},
    {UnrootedAT::QUERY_NODE, 2000},   {UnrootedAT::UPDATE_NODE, 2500},
    {UnrootedAT::UPDATE_SUBTREE, 0},  {UnrootedAT::QUERY_SUBTREE, 3500},
    {UnrootedAT::SUBTREE_SIZE, 1500},
};
actions_t<UnrootedAT> subtree_speed_update_heavy_actions = {
    {UnrootedAT::LINK, 1500},        {UnrootedAT::CUT, 300},
    {UnrootedAT::LINK_CUT, 1200},    {UnrootedAT::FINDROOT, 1000},
    {UnrootedAT::LCA, 800},          {UnrootedAT::LCA_CONN, 1200},
    {UnrootedAT::QUERY_NODE, 1000},  {UnrootedAT::UPDATE_NODE, 6000},
    {UnrootedAT::UPDATE_SUBTREE, 0}, {UnrootedAT::QUERY_SUBTREE, 2400},
    {UnrootedAT::SUBTREE_SIZE, 600},
};
actions_t<UnrootedAT> subtree_speed_query_heavy_actions = {
    {UnrootedAT::LINK, 1500},         {UnrootedAT::CUT, 300},
    {UnrootedAT::LINK_CUT, 1200},     {UnrootedAT::FINDROOT, 1000},
    {UnrootedAT::LCA, 800},           {UnrootedAT::LCA_CONN, 1200},
    {UnrootedAT::QUERY_NODE, 5000},   {UnrootedAT::UPDATE_NODE, 1200},
    {UnrootedAT::UPDATE_SUBTREE, 0},  {UnrootedAT::QUERY_SUBTREE, 10000},
    {UnrootedAT::SUBTREE_SIZE, 4000},
};

void speed_test_lct_subtree(int N, const actions_t<UnrootedAT>& freq) {
    lct_subtree tree(N);
    auto actions = make_unrooted_actions(N, 10s, freq, N - 100);

    START(linkcut);
    for (const auto& [action, u, v, r, who, val] : actions) {
        bool ok = true;
        switch (action) {
        case UnrootedAT::LINK: {
            tree.link(u, v);
        } break;
        case UnrootedAT::CUT: {
            tree.cut(u, v);
        } break;
        case UnrootedAT::FINDROOT: {
            tree.reroot(who);
            int ans = tree.findroot(u);
            ok = ans == who;
        } break;
        case UnrootedAT::LCA: {
            tree.reroot(r);
            int ans = tree.lca(u, v);
            ok = ans == who;
        } break;
        case UnrootedAT::QUERY_NODE: {
            long ans = tree.access_node(u)->self;
            ok = val == ans;
        } break;
        case UnrootedAT::UPDATE_NODE: {
            tree.access_node(u)->self = val;
        } break;
        case UnrootedAT::QUERY_SUBTREE: {
            long ans = tree.access_subtree(u, v)->subtree();
            ok = val == ans;
        } break;
        case UnrootedAT::SUBTREE_SIZE: {
            long ans = tree.access_subtree(u, v)->subtree_size();
            ok = val == ans;
        } break;
        // case UnrootedAT::UPDATE_SUBTREE: {
        //     tree.access_subtree(u, v)->lazy += val;
        // } break;
        default:
            throw runtime_error("Unsupported action");
        }
        if (!ok) {
            printcl("Failed action: {}\n", action_names<UnrootedAT>.at(action));
        }
        assert(ok);
    }
    TIME(linkcut);
    PRINT_EACH_NS(linkcut, actions.size());
}

} // namespace speed_testing_lct_subtree

int main() {
    setbuf(stdout, nullptr);
    RUN_SHORT(stress_test_lct_subtree());
    RUN_SHORT(stress_test_link_cut_tree_path());
    RUN_SHORT(speed_test_lct_path(25000, path_speed_query_heavy_actions));
    RUN_SHORT(speed_test_lct_path(25000, path_speed_update_heavy_actions));
    RUN_SHORT(speed_test_lct_path(25000, path_speed_topo_heavy_actions));
    RUN_SHORT(speed_test_lct_subtree(25000, subtree_speed_query_heavy_actions));
    RUN_SHORT(speed_test_lct_subtree(25000, subtree_speed_update_heavy_actions));
    RUN_SHORT(speed_test_lct_subtree(25000, subtree_speed_topo_heavy_actions));
    return 0;
}
