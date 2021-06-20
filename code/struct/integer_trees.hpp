#pragma once

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
 * 	 Complexity: O(I log N), I=#deleted intervals. O(log N) amortized.
 *
 * You can exclude() an interval I, which will erase any existing intervals contained in
 * it and splice any intervals otherwise overlapping I (at most 2).
 *   Complexity: O(E log N), E=#deleted intervals. O(log N) amortized.
 *
 * You can toggle() an interval I, which will splice overlapping intervals (at most 2)
 * and intuitively "toggle" all of the numbers inside I and its containing intervals.
 *   Complexity: O(T log N), T=#toggled intervals. N increases by at most 1.
 */
template <typename T>
struct merging_interval_tree : set<array<T, 2>> {
    using interval_t = array<T, 2>;
    using base_t = set<interval_t>;

    void add(interval_t intv) {
        auto& [L, R] = intv;
        assert(L < R);
        auto lo = base_t::lower_bound({L, L});
        if (lo != base_t::end() && L >= (*lo)[0]) {
            ++lo;
        }
        if (lo != base_t::begin() && (*prev(lo))[1] >= L) {
            L = min(L, (*--lo)[0]);
        }
        auto hi = base_t::lower_bound({R, R});
        if (hi != base_t::end() && R >= (*hi)[0]) {
            ++hi;
        }
        if (hi != base_t::begin()) {
            R = max(R, (*prev(hi))[1]);
        }
        base_t::erase(lo, hi);
        base_t::insert(intv);
    }

    void exclude(interval_t intv) {
        auto& [L, R] = intv;
        assert(L < R);
        vector<interval_t> around;
        auto lo = base_t::lower_bound({L, L});
        if (lo != base_t::begin() && (*prev(lo))[1] >= L) {
            around.push_back({(*--lo)[0], L});
        }
        auto hi = base_t::lower_bound({R, R});
        if (hi != base_t::begin() && (*prev(hi))[1] > R) {
            around.push_back({R, (*prev(hi))[1]});
        }
        base_t::erase(lo, hi);
        base_t::insert(around.begin(), around.end());
    }

    optional<interval_t> get_interval(T num) const {
        auto it = base_t::lower_bound({num, num});
        if (it != base_t::end() && num >= (*it)[0])
            return *it;
        if (it != base_t::begin() && (*prev(it))[0] <= num && num < (*prev(it))[1]) {
            return *prev(it);
        }
        return nullopt;
    }

    bool contains(T num) const { return get_interval(num).has_value(); }

    bool contains(interval_t intv) const {
        auto wrap = get_interval(intv[0]);
        return wrap.has_value() && (*wrap)[1] >= intv[1];
    }

    bool overlaps(interval_t intv) const {
        auto lo = base_t::lower_bound({intv[0], intv[0]});
        return !(lo == base_t::end() || (*lo)[0] >= intv[1]) ||
               !(lo == base_t::begin() || (*--lo)[1] <= intv[0]);
    }

    T cover_length() const {
        T sum = 0;
        for (const auto& [lo, hi] : *this)
            sum += hi - lo;
        return sum;
    }
};

template <typename T, typename... Rs>
struct multiset_rollback : multiset<T, Rs...> {
    using base_t = multiset<T, Rs...>;

    enum Operation { INSERT, ERASE };
    vector<pair<Operation, T>> history;

    template <typename... Args>
    explicit multiset_rollback(Args&&... args) : base_t(forward<Args>(args)...) {}

    int time() const { return history.size(); }

    void rollback(int t) {
        int i = time();
        while (i-- > t) {
            const auto& [op, value] = history.back();
            if (op == INSERT) {
                base_t::erase(base_t::find(value));
            } else if (op == ERASE) {
                base_t::insert(value);
            }
            history.pop_back();
        }
    }

    void insert(const T& value) {
        history.emplace_back(INSERT, value);
        base_t::insert(value);
    }

    void erase(const T& value) {
        if (auto it = base_t::find(value); it != base_t::end()) {
            history.emplace_back(ERASE, value);
            base_t::erase(it);
        }
    }

    void clear() {
        for (const auto& value : *this) {
            history.emplace_back(ERASE, value);
        }
        base_t::clear();
    }
};

template <typename Key, typename Value, typename... Rs>
struct map_rollback : map<Key, Value, Rs...> {
    using base_t = map<Key, Value, Rs...>;

    enum Operation { INSERT, MODIFY, ERASE };
    vector<tuple<Operation, Key, Value>> history;

    template <typename... Args>
    explicit map_rollback(Args&&... args) : base_t(forward<Args>(args)...) {}

    int time() const { return history.size(); }

    void rollback(int t) {
        int i = time();
        while (i-- > t) {
            const auto& [op, key, value] = history.back();
            if (op == INSERT) {
                base_t::erase(key);
            } else if (op == ERASE) {
                base_t::emplace(key, value);
            } else if (op == MODIFY) {
                base_t::find(key)->second = value;
            }
            history.pop_back();
        }
    }

    Value& operator[](const Key& key) {
        if (auto it = base_t::find(key); it != base_t::end()) {
            history.emplace_back(MODIFY, key, it->second);
            return it->second;
        } else {
            history.emplace_back(INSERT, key, Value());
            return base_t::emplace(key, Value()).first->second;
        }
    }

    Value& at(const Key& key) {
        const auto& kv = base_t::at(key);
        history.emplace_back(MODIFY, key, kv.second);
        return kv.second;
    }

    const Value& at(const Key& key) const {
        const auto& kv = base_t::at(key);
        return kv.second;
    }

    const Value& get(const Key& key) const {
        const auto& kv = base_t::at(key);
        return kv.second;
    }

    void erase(const Key& key) {
        if (auto it = base_t::find(key); it != base_t::end()) {
            history.emplace_back(ERASE, it->first, it->second);
            base_t::erase(it);
        }
    }

    void clear() {
        for (const auto& value : *this) {
            history.emplace_back(ERASE, value);
        }
        base_t::clear();
    }
};
