#include "test_utils.hpp"
// #include "../struct/euler_tour_tree_crtp.hpp"
#include "../struct/euler_tour_tree.hpp"
#include "lib/tree_action.hpp"

using ett_subtree = euler_tour_tree<ett_node_sum>;
using namespace tree_testing;

inline namespace detail {

bool stress_verify_ett(slow_tree& slow, ett_subtree& tree, int D = 2) {
    assert(1 <= D && D <= 9);
    int N = slow.N;

    vector<vector<int>> subtree(D), subtree_size(D);
    vector<int> exp_subtree(N + 1), exp_subtree_size(N + 1);
    vector<int> nodes(N);
    iota(begin(nodes), end(nodes), 1);

    bool ok_subtree = true, ok_subtree_size = true;

    for (int u = 1; u <= N; u++) {
        exp_subtree[u] = slow.query_subtree(u);
        exp_subtree_size[u] = slow.subtree_size(u);
    }
    for (int d = 0; d < 0; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        subtree[d].resize(N + 1);
        for (int u : order) { subtree[d][u] = tree.access_subtree(u)->subtree(); }
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
            subtree_size[d][u] = tree.access_subtree(u)->subtree_size();
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

inline namespace stress_testing_euler_tour_tree {

actions_t<RootedAT> stress_path_ratio = {
    {RootedAT::LINK, 2500},           {RootedAT::CUT, 500},
    {RootedAT::LINK_CUT, 2000},       {RootedAT::LCA, 0},
    {RootedAT::FINDROOT, 0},          {RootedAT::LCA_CONN, 0},
    {RootedAT::QUERY_NODE, 2000},     {RootedAT::UPDATE_NODE, 3000},
    {RootedAT::UPDATE_SUBTREE, 3500}, {RootedAT::QUERY_SUBTREE, 3500},
    {RootedAT::SUBTREE_SIZE, 1500},   {RootedAT::STRESS_TEST, 400},
};

void stress_test_euler_tour_tree(int N = 200) {
    slow_tree slow(N, true);
    ett_subtree tree(N);
    auto actions = make_rooted_actions(N, 400ms, stress_path_ratio, 9 * N / 10);
    bool ok = true;

    for (int ia = 0, A = actions.size(); ia < A; ia++) {
        print_regular(ia, A, 1000, "stress test euler tour tree");
        auto [action, u, v, r, who, val] = actions[ia];
        string label;

        switch (action) {
        case RootedAT::LINK: {
            slow.link(u, v);
            tree.link(u, v);
            label = format("[{}] LINK {}--{}", slow.S, u, v);
        } break;
        case RootedAT::CUT: {
            slow.cut(u);
            tree.cut(u);
            label = format("[{}] CUT {}", slow.S, u);
        } break;
        // case RootedAT::FINDROOT: {
        //     tree.reroot(r);
        //     slow.reroot(r);
        //     int ans = tree.findroot(u);
        //     ok = ans == r;
        //     label = format("[{}] FINDROOT {}: ={} ?{}\n", slow.S, u, r, ans);
        // } break;
        // case RootedAT::LCA: {
        //     int ans = tree.lca(u, v);
        //     label = format("[{}] LCA {}..{}, {}: ={} ?{}", slow.S, u, v, r, w, ans);
        //     ok = ans == w;
        // } break;
        case RootedAT::QUERY_NODE: {
            long ans = tree.access_node(u)->self;
            ok = val == ans;
            label = format("[{}] QUERY {}: ={} ?{}", slow.S, u, val, ans);
        } break;
        case RootedAT::UPDATE_NODE: {
            long init = slow.query_node(u);
            slow.update_node(u, val);
            tree.access_node(u)->self = val;
            label = format("[{}] UPDATE {}: {}->{}", slow.S, u, init, val);
        } break;
        case RootedAT::UPDATE_SUBTREE: {
            slow.update_subtree(u, val);
            tree.access_subtree(u)->lazy += val;
            label = format("[{}] UPDATE SUBTREE {}: {:+}", slow.S, u, val);
        } break;
        case RootedAT::QUERY_SUBTREE: {
            long ans = tree.access_subtree(u)->subtree();
            ok = val == ans;
            label = format("[{}] QUERY SUBTREE {}: ={} ?{}", slow.S, u, val, ans);
        } break;
        case RootedAT::SUBTREE_SIZE: {
            long ans = tree.access_subtree(u)->subtree_size();
            ok = val == ans;
            label = format("[{}] SUBTREE SIZE {}: ={} ?{}", slow.S, u, val, ans);
        } break;
        case RootedAT::STRESS_TEST: {
            ok = stress_verify_ett(slow, tree);
            label = format("[{}] STRESS TEST", slow.S);
        } break;
        default:
            throw runtime_error("Unsupported action");
        }

        assert(ok);
    }

    assert(ok);
}

} // namespace stress_testing_euler_tour_tree

inline namespace unit_testing_euler_tour_tree {

void unit_test_euler_tour_tree() {
    ett_subtree ett(10);
    auto test_in_subtree = [&](int u, int v) {
        print("in_subtree({},{}): {}\n", u, v, ett.in_subtree(u, v));
    };
    auto test_subtree = [&](int u) {
        print("subtree({}): {}\n", u, ett.access_subtree(u)->subtree());
    };
    auto test_subt_size = [&](int u) {
        print("subt_size({}): {}\n", u, ett.access_subtree(u)->subtree_size());
    };
    ett.link(1, 2);
    ett.link(2, 3);
    ett.link(4, 7);
    ett.link(5, 3);
    ett.link(6, 3);
    ett.link(7, 6);
    test_in_subtree(7, 3);
    test_in_subtree(7, 2);
    test_in_subtree(4, 3);
    test_in_subtree(5, 3);
    test_in_subtree(5, 2);
    test_in_subtree(3, 6);
    test_in_subtree(4, 7);
    test_in_subtree(7, 6);
    test_in_subtree(6, 3);
    ett.access_node(3)->self = 70;
    ett.access_node(8)->self = 30;
    ett.access_node(7)->self = 20;
    ett.access_node(2)->self = 10;
    ett.access_subtree(6)->lazy += 50;
    test_subt_size(6);
    test_subt_size(3);
    test_subtree(3);
    test_subtree(6);
    test_subtree(7);
    test_subtree(4);
    test_subtree(9);
}

} // namespace unit_testing_euler_tour_tree

inline namespace speed_testing_euler_tour_tree {

actions_t<RootedAT> speed_topo_heavy = {
    {RootedAT::LINK, 5000},           {RootedAT::CUT, 1000},
    {RootedAT::LINK_CUT, 4000},       {RootedAT::LCA, 0},
    {RootedAT::FINDROOT, 0},          {RootedAT::LCA_CONN, 0},
    {RootedAT::QUERY_NODE, 2000},     {RootedAT::UPDATE_NODE, 2500},
    {RootedAT::UPDATE_SUBTREE, 2500}, {RootedAT::QUERY_SUBTREE, 3500},
    {RootedAT::SUBTREE_SIZE, 1500},
};
actions_t<RootedAT> speed_update_heavy = {
    {RootedAT::LINK, 1500},           {RootedAT::CUT, 500},
    {RootedAT::LINK_CUT, 1000},       {RootedAT::LCA, 0},
    {RootedAT::FINDROOT, 0},          {RootedAT::LCA_CONN, 0},
    {RootedAT::QUERY_NODE, 1000},     {RootedAT::UPDATE_NODE, 6000},
    {RootedAT::UPDATE_SUBTREE, 8000}, {RootedAT::QUERY_SUBTREE, 2400},
    {RootedAT::SUBTREE_SIZE, 600},
};
actions_t<RootedAT> speed_query_heavy = {
    {RootedAT::LINK, 1500},           {RootedAT::CUT, 500},
    {RootedAT::LINK_CUT, 1000},       {RootedAT::LCA, 0},
    {RootedAT::FINDROOT, 0},          {RootedAT::LCA_CONN, 0},
    {RootedAT::QUERY_NODE, 5000},     {RootedAT::UPDATE_NODE, 1200},
    {RootedAT::UPDATE_SUBTREE, 1800}, {RootedAT::QUERY_SUBTREE, 10000},
    {RootedAT::SUBTREE_SIZE, 4000},
};

void speed_test_euler_tour_tree(int N, const actions_t<RootedAT>& freq) {
    ett_subtree tree(N);
    auto actions = make_rooted_actions(N, 10s, freq, N - 100);

    START(ett);
    for (const auto& [action, u, v, r, w, val] : actions) {
        bool ok = true;
        switch (action) {
        case RootedAT::LINK: {
            tree.link(u, v);
        } break;
        case RootedAT::CUT: {
            tree.cut(u);
        } break;
        // case RootedAT::LCA: {
        //     tree.reroot(r);
        //     int ans = tree.lca(u, v);
        //     assert(ans == w);
        // } break;
        // case RootedAT::FINDROOT: {
        //     tree.reroot(r);
        //     int ans = tree.findroot(u);
        //     assert(ans == r);
        // } break;
        case RootedAT::QUERY_NODE: {
            long ans = tree.access_node(u)->self;
            ok = val == ans;
        } break;
        case RootedAT::UPDATE_NODE: {
            tree.access_node(u)->self = val;
        } break;
        case RootedAT::UPDATE_SUBTREE: {
            tree.access_subtree(u)->lazy += val;
        } break;
        case RootedAT::QUERY_SUBTREE: {
            long ans = tree.access_subtree(u)->subtree();
            ok = val == ans;
        } break;
        case RootedAT::SUBTREE_SIZE: {
            long ans = tree.access_subtree(u)->subtree_size();
            ok = val == ans;
        } break;
        default:
            throw runtime_error("Unsupported action");
        }
        assert(ok);
    }
    TIME(ett);
    PRINT_EACH_NS(ett, actions.size());
}

} // namespace speed_testing_euler_tour_tree

int main() {
    RUN_SHORT(unit_test_euler_tour_tree());
    RUN_BLOCK(stress_test_euler_tour_tree());
    RUN_BLOCK(speed_test_euler_tour_tree(25000, speed_query_heavy));
    RUN_BLOCK(speed_test_euler_tour_tree(25000, speed_update_heavy));
    RUN_BLOCK(speed_test_euler_tour_tree(25000, speed_topo_heavy));
    return 0;
}
