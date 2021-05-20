#ifndef INTEGER_TREES_HPP
#define INTEGER_TREES_HPP

#include <bits/stdc++.h>
using namespace std;

template <typename T>
struct merging_interval_tree {
    using interval_t = array<T, 2>;
    set<interval_t> tree;

    merging_interval_tree() = default;

    void insert(interval_t intv) {
        assert(intv[0] < intv[1]);
        // Lower bound
        auto lo = tree.lower_bound({intv[0], intv[0]});
        if (lo != tree.end() && intv[0] >= (*lo)[0]) {
            ++lo;
        }
        if (lo != tree.begin() && (*prev(lo))[1] >= intv[0]) {
            intv[0] = min(intv[0], (*--lo)[0]);
        }
        // Upper bound
        auto hi = tree.lower_bound({intv[1], intv[1]});
        if (hi != tree.end() && intv[1] >= (*hi)[0]) {
            ++hi;
        }
        if (hi != tree.begin()) {
            intv[1] = max(intv[1], (*prev(hi))[1]);
        }
        tree.erase(lo, hi);
        tree.insert(intv);
    }

    void exclude(interval_t intv) {
        assert(intv[0] < intv[1]);
        vector<interval_t> backin;
        // Lower bound
        auto lo = tree.lower_bound({intv[0], intv[0]});
        if (lo != tree.begin() && (*prev(lo))[1] >= intv[0]) {
            backin.push_back({(*--lo)[0], intv[0]});
        }
        // Upper bound
        auto hi = tree.lower_bound({intv[1], intv[1]});
        if (hi != tree.begin() && (*prev(hi))[1] > intv[1]) {
            backin.push_back({intv[1], (*prev(hi))[1]});
        }
        tree.erase(lo, hi);
        tree.insert(backin.begin(), backin.end());
    }

    bool contains(T num) {
        auto it = tree.lower_bound({num, num});
        if (it != tree.end() && num >= (*it)[0]) {
            return true;
        } else if (it != tree.begin()) {
            return (*prev(it))[0] <= num && num < (*prev(it))[1];
        } else {
            return false;
        }
    }

    T cover_length() const {
        T sum = 0;
        for (const auto& [lo, hi] : tree) {
            sum += hi - lo;
        }
        return sum;
    }

    int size() const { return tree.size(); }

    bool verify() const {
        if (!tree.empty()) {
            auto [lo, hi] = *tree.begin();
            if (lo >= hi) {
                return false;
            }
            for (auto it = next(tree.begin()); it != tree.end(); ++it) {
                auto [nlo, nhi] = *it;
                if (hi >= nlo || nlo >= nhi) {
                    return false;
                }
                tie(lo, hi) = make_pair(nlo, nhi);
            }
        }
        return true;
    }

    auto begin() const { return tree.begin(); }
    auto end() const { return tree.end(); }
    auto rbegin() const { return tree.rbegin(); }
    auto rend() const { return tree.rend(); }

    friend string to_string(const merging_interval_tree& mit) {
        return to_string(mit.tree);
    }
    friend ostream& operator<<(ostream& out, const merging_interval_tree& mit) {
        return out << mit.tree;
    }
};

#endif // INTEGER_TREES_HPP
