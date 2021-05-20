#ifndef INTEGER_TREES_HPP
#define INTEGER_TREES_HPP

#include <bits/stdc++.h>
using namespace std;

/**
 * A tree containing a collection of non-empty intervals between an unspecified range
 * All intervals are left-inclusive and right-exclusive; if T is an integral type,
 * single integer intervals look like [n,n+1); if T is a floating point type this does not
 * matter much, but there is no epsilon handling so there is a potential for very small
 * intervals to appear.
 *
 * Let N be the number of existing intervals.
 * You can insert() an interval I, and it will be merged with overlapping ones.
 * 	 Complexity: O(I log N), I=#inserted intervals. O(log N) amortized.
 *
 * You can exclude() an interval I, which will erase any existing intervals contained in
 * it and splice any intervals otherwise overlapping I (at most 2).
 *   Complexity: O(E log N), E=#deleted intervals. O(log N) amortized.
 *
 * You can toggle() and interval I, which will splice overlapping intervals (at most 2)
 * and intuitively "toggle" all of the numbers inside I and its containing intervals.
 *   Complexity: O(T log N), T=#toggled intervals. N increases by at most 1.
 */
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
        vector<interval_t> out;
        // Lower bound
        auto lo = tree.lower_bound({intv[0], intv[0]});
        if (lo != tree.begin() && (*prev(lo))[1] >= intv[0]) {
            out.push_back({(*--lo)[0], intv[0]});
        }
        // Upper bound
        auto hi = tree.lower_bound({intv[1], intv[1]});
        if (hi != tree.begin() && (*prev(hi))[1] > intv[1]) {
            out.push_back({intv[1], (*prev(hi))[1]});
        }
        tree.erase(lo, hi);
        tree.insert(out.begin(), out.end());
    }

    void toggle(interval_t intv) {
        assert(intv[0] < intv[1]);
        vector<interval_t> out, in;
        // Lower bound
        auto lo = tree.lower_bound({intv[0], intv[0]});
        if (lo != tree.begin() && (*prev(lo))[1] >= intv[0]) {
            out.push_back({(*--lo)[0], intv[0]});
        }
        // Upper bound
        auto hi = tree.lower_bound({intv[1], intv[1]});
        if (hi != tree.begin() && (*prev(hi))[1] > intv[1]) {
            out.push_back({intv[1], (*prev(hi))[1]});
        } else if (hi != tree.end() && (*hi)[0] <= intv[1]) {
            out.push_back({intv[1], (*hi++)[1]});
        }
        // Generate intermediate intervals
        T cur = intv[0];
        for (auto it = lo; it != hi; ++it) {
            auto [a, b] = *it;
            if (cur < a) {
                in.push_back({cur, a});
            }
            cur = max(cur, b);
        }
        if (cur < intv[1]) {
            in.push_back({cur, intv[1]});
        }
        int s = out.size(), t = in.size();
        if (s && t && out[0][0] <= in[0][0] && in[0][0] <= out[0][1]) {
            in[0][0] = min(in[0][0], out[0][0]);
            out.erase(out.begin()), s--;
        }
        if (s && t && out[s - 1][0] <= in[t - 1][1] && in[t - 1][1] <= out[s - 1][1]) {
            in[t - 1][1] = max(in[t - 1][1], out[s - 1][1]);
            out.erase(out.begin() + (s - 1));
        }
        tree.erase(lo, hi);
        tree.insert(out.begin(), out.end());
        tree.insert(in.begin(), in.end());
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
