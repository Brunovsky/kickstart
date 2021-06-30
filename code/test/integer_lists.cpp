#include "test_utils.hpp"
#include "../struct/integer_lists.hpp"

void unit_test_forward_lists() {
    forward_lists fl(6, 20);
    //                 0  1  2  3  4  5
    fl.push(2, 3); //  3     3
    fl.push(2, 4); //  4     4
    fl.push(2, 7); //  7     7
    fl.push(2, 9); //        9
    fl.push(3, 5); //           5     5
    fl.push(4, 8); //              8
    fl.push(3, 1); //           1     1
    fl.push(2, 0); //        0
    fl.push(1, 2); //     2
    fl.push(3, 6); //           6

    vector<vector<int>> seen(6);
    for (int l = 0; l < 6; l++)
        FOR_EACH_IN_FORWARD_LIST (i, l, fl)
            seen[l].push_back(i);

    // print("forward_lists\n"), debugh(seen);
    assert(seen[0] == vector<int>({}));
    assert(seen[1] == vector<int>({2}));
    assert(seen[2] == vector<int>({0, 9, 7, 4, 3}));
    assert(seen[3] == vector<int>({6, 1, 5}));
    assert(seen[4] == vector<int>({8}));
    assert(seen[5] == vector<int>({}));
}

void battle_test_linked_lists() {
    constexpr int L = 30, N = 5000;
    intd distL(0, L - 1);
    intd distN(0, N - 1);
    intd actiond(0, 99);
    linked_lists ll(L, N);
    unordered_set<int> lists[L];
    unordered_map<int, int> all;
    mt.seed(time(nullptr));

    auto walk = [&](int l) {
        vector<int> a;
        FOR_EACH_IN_LINKED_LIST (i, l, ll)
            a.push_back(i);
        sort(begin(a), end(a));
        return a;
    };
    auto sorted = [&](int l) {
        vector<int> a(begin(lists[l]), end(lists[l]));
        sort(begin(a), end(a));
        return a;
    };
    auto verify = [&]() {
        for (int l = 0; l < L; l++) {
            if (walk(l) != sorted(l))
                return false;
        }
        return true;
    };

    LOOP_FOR_DURATION_TRACKED (4s, now) {
        print_time(now, 4s, "battle test linked list");

        int a = actiond(mt);
        if (a < 40) {
            int l = distL(mt), n = distN(mt), b = actiond(mt);
            if (all.count(n)) {
                ll.erase(n);

                lists[all.at(n)].erase(n), all.erase(n);
            } else {
                if (lists[l].empty()) {
                    ll.init(l, n);
                } else if (b < 30) {
                    ll.push_front(l, n);
                } else if (b < 60) {
                    ll.push_back(l, n);
                } else if (b < 80) {
                    ll.insert_after(ll.head(l), n);
                } else {
                    ll.insert_before(ll.tail(l), n);
                }

                all[n] = l, lists[l].insert(n);
            }
        } else if (a < 95) {
            int l = distL(mt), b = actiond(mt), n;
            if (!lists[l].empty()) {
                if (b < 50) {
                    n = ll.head(l);
                    ll.pop_front(l);
                } else {
                    n = ll.tail(l);
                    ll.pop_back(l);
                }

                assert(lists[l].count(n) && all.at(n) == l);
                all.erase(n), lists[l].erase(n);
            }
        } else {
            int l = distL(mt);
            ll.clear(l);

            for (int n : lists[l])
                all.erase(n);
            lists[l].clear();
        }

        if (actiond(mt) < 5) {
            assert(verify());
        }
    }
    assert(verify());
}

int main() {
    RUN_SHORT(unit_test_forward_lists());
    RUN_BLOCK(battle_test_linked_lists());
    return 0;
}
