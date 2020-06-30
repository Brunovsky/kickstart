#include <bits/stdc++.h>

using namespace std;

// *****

struct soldier_t {
    int A, D;

    friend bool operator==(soldier_t lhs, soldier_t rhs) {
        return lhs.A == rhs.A && lhs.D == rhs.D;
    }
};

namespace std {
template <>
struct hash<soldier_t> {
    size_t operator()(soldier_t soldier) const {
        return (size_t(soldier.A) << 32) | (size_t(soldier.D));
    }
};
} // namespace std

int N;
vector<soldier_t> soldiers;
list<int> As;
list<int> Ds;
unordered_map<soldier_t, bool> W;

bool A_cmp(int i, int j) {
    int Ai = soldiers[i].A, Aj = soldiers[j].A;
    return Ai < Aj || (Ai == Aj && i < j);
}

bool D_cmp(int i, int j) {
    int Di = soldiers[i].D, Dj = soldiers[j].D;
    return Di < Dj || (Di == Dj && i < j);
}

bool unobtainable(soldier_t s, soldier_t bound) {
    return s.A <= bound.A && s.D <= bound.D;
}

void splice_A(list<int> &E, soldier_t bound) {
    auto it = As.begin();
    while (it != As.end()) {
        if (unobtainable(soldiers[*it], bound)) {
            auto begin = it;
            ++it;
            while (it != As.end() && unobtainable(soldiers[*it], bound))
                ++it;
            E.splice(E.end(), As, begin, it);
        } else {
            if (soldiers[*it].A > bound.A)
                return;
            ++it;
        }
    }
}

void splice_D(list<int> &E, soldier_t bound) {
    auto it = Ds.begin();
    while (it != Ds.end()) {
        if (unobtainable(soldiers[*it], bound)) {
            auto begin = it;
            ++it;
            while (it != Ds.end() && unobtainable(soldiers[*it], bound))
                ++it;
            E.splice(E.end(), Ds, begin, it);
        } else {
            if (soldiers[*it].D > bound.D)
                return;
            ++it;
        }
    }
}

void recover_A(list<int> &E) {
    As.merge(E, A_cmp);
}

void recover_D(list<int> &E) {
    Ds.merge(E, D_cmp);
}

bool check(soldier_t bound) {
    if (W.count(bound))
        return W.at(bound);

    // final position, no moves allowed
    if (As.empty() && Ds.empty()) {
        return W[bound] = false;
    }
    // if no available soldiers may be picked through A (or D), then
    // picking the remaining soldier with maximum D (or A) wins
    if (As.empty() || Ds.empty()) {
        return W[bound] = true;
    }
    // if there is a minimum or maximum soldier for A and D, this wins
    if (As.front() == Ds.front() || As.back() == Ds.back()) {
        return W[bound] = true;
    }

    // otherwise recurse
    // pick any soldier available through A
    for (auto rit = As.begin(); rit != As.end(); ++rit) {
        int i = *rit;
        soldier_t soldier = soldiers[i];
        soldier_t rbound = {max(bound.A, soldier.A), max(bound.D, soldier.D)};
        list<int> erased_As, erased_Ds;
        splice_A(erased_As, rbound);
        splice_D(erased_Ds, rbound);
        bool is_winning = check(rbound);
        recover_A(erased_As);
        recover_D(erased_Ds);
        if (!is_winning) {
            return W[bound] = true;
        }
    }
    // pick any soldier available through D
    for (auto rit = Ds.begin(); rit != Ds.end(); ++rit) {
        int i = *rit;
        soldier_t soldier = soldiers[i];
        soldier_t rbound = {max(bound.A, soldier.A), max(bound.D, soldier.D)};
        list<int> erased_As, erased_Ds;
        splice_A(erased_As, rbound);
        splice_D(erased_Ds, rbound);
        bool is_winning = check(rbound);
        recover_A(erased_As);
        recover_D(erased_Ds);
        if (!is_winning) {
            return W[bound] = true;
        }
    }
    return W[bound] = false;
}

auto solve() {
    cin >> N >> ws;
    soldiers.resize(N);
    for (int i = 0; i < N; ++i)
        cin >> soldiers[i].A >> soldiers[i].D >> ws;

    As.resize(N);
    Ds.resize(N);
    iota(As.begin(), As.end(), 0);
    iota(Ds.begin(), Ds.end(), 0);
    As.sort(A_cmp);
    Ds.sort(D_cmp);
    W.clear();

    return check({0, 0}) ? "YES" : "NO";
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
