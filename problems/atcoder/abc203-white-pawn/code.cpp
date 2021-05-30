#include <bits/stdc++.h>

using namespace std;

#define long int64_t

// *****

inline namespace lib {

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

    bool contains(T num) const {
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

    bool empty() const { return tree.empty(); }
    int size() const { return tree.size(); }

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

} // namespace lib

auto solve() {
    int N, M;
    cin >> N >> M;
    unordered_map<int, set<int>> by_column;
    vector<array<int, 2>> blacks(M);
    for (auto& [x, y] : blacks) {
        cin >> x >> y;
        by_column[y].insert(x);
    }

    unordered_map<int, merging_interval_tree<int>> tags;
    // we start at (0,N), last row is (2N,y)

    auto add_forward = [&](int x, int y) {
        if (!by_column.count(y)) {
            tags[y].insert({x, 2 * N + 1});
        } else {
            auto it = by_column.at(y).upper_bound(x);
            if (it == by_column.at(y).end()) {
                tags[y].insert({x, 2 * N + 1});
            } else {
                tags[y].insert({x, *it});
            }
        }
    };

    auto can_reach = [&](int x, int y) {
        if (!tags.count(y)) {
            return false;
        } else {
            return tags.at(y).contains(x);
        }
    };

    add_forward(0, N);
    sort(begin(blacks), end(blacks));

    for (auto [x, y] : blacks) {
        if (can_reach(x - 1, y - 1) || can_reach(x - 1, y + 1)) {
            add_forward(x, y);
        }
    }

    long ans = 0;
    for (const auto& [col, tree] : tags) {
        ans += tree.contains(2 * N);
    }
    return ans;
}

// *****

int main() {
    ios::sync_with_stdio(false);
    cout << solve() << endl;
    return 0;
}
