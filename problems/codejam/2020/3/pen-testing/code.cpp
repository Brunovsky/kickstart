#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXT              100'000
#define MAXN              15
#define MAXMASK           (1 << MAXN)
#define CONTAINS(mask, i) (mask & (1 << i))
#define EXCEPT(mask, i)   (mask ^ (1 << i))

enum Strategy { NONE = 0, PICK, DRAIN_ONE, DRAIN_ALL };

// solver data
int T, N, C;
int drivers_done = 0;
int judge_round = 0;

Strategy mask_strategy[MAXMASK];
double mask_optimal[MAXMASK];

inline void next_lexicographical_mask(int& v) {
    int t = v | (v - 1);
    v = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
}

void compute_strategy(int mask) {
    int cnt = __builtin_popcount(mask);
    int combs = cnt * (cnt - 1) / 2;

    // Strategy PICK: Pick two random pens remaining
    int good = 0;
    for (int i = 0; i < N; i++) {
        if (CONTAINS(mask, i)) {
            for (int j = i + 1; j < N; j++) {
                if (CONTAINS(mask, j)) {
                    good += i + j >= N;
                }
            }
        }
    }
    double pick = 1.0 * good / combs;

    // Strategy DRAIN_ONE: Completely drain one random pen
    double sum = 0;
    for (int i = 0; i < N; i++) {
        if (CONTAINS(mask, i)) {
            sum += mask_optimal[EXCEPT(mask, i)];
        }
    }
    double drain_one = sum / cnt;

    // Strategy DRAIN_ALL: Drain once all the pens
    double drain_all = mask_optimal[mask >> 1];

    // Pick the highest probability of winning
    double best = max(pick, max(drain_one, drain_all));

    if (pick >= best) {
        mask_strategy[mask] = PICK;
    } else if (drain_one >= best) {
        mask_strategy[mask] = DRAIN_ONE;
    } else if (drain_all >= best) {
        mask_strategy[mask] = DRAIN_ALL;
    } else {
        abort();
    }

    mask_optimal[mask] = best;
}

void prepare() {
    const int maxmask = 1 << N;
    for (int pens = 2; pens <= N; pens++) {
        int mask = (1 << pens) - 1;
        do {
            compute_strategy(mask);
            next_lexicographical_mask(mask);
        } while (mask < maxmask);
    }
}

string bits(int mask) {
    string s;
    for (int i = 0; i < N; i++) {
        s += CONTAINS(mask, i) ? '1' : '0';
    }
    return s;
}

struct Driver {
    int mask, t;
    Strategy strategy;
    int idx, ink = 0, vis = 0;
    int query = 0, answer = 0, chosen[2];

    Driver() = default;
    Driver(int t, int N) : t(t) {
        mask = idx = (1 << N) - 1;
        strategize();
    }

    void update() {
        if (strategy == DRAIN_ONE) {
            drain_one();
        } else if (strategy == DRAIN_ALL) {
            drain_all();
        }
    }

    void pick() {
        // pick two indices
        int lo1 = __builtin_ctz(idx);
        int rest = EXCEPT(idx, lo1);
        int lo2 = __builtin_ctz(rest);
        chosen[0] = lo1;
        chosen[1] = lo2;
        query = -1;
        drivers_done++;
        check();
    }

    void drain_one() {
        // the pen is now empty, we drained it ink times
        if (answer == 0) {
            idx = EXCEPT(idx, query);
            mask = EXCEPT(mask, ink);
            ink = 0;
            return strategize();
        }
        // we have drained the pen one ink, do it again
        ink++;
        check();
    }

    void drain_all() {
        vis |= 1 << query;
        // check if we drained the pen with 0 ink
        if (answer == 0) {
            idx = EXCEPT(idx, query);
            vis = EXCEPT(vis, query);
        }
        // we have drained all the pens
        if (vis == idx) {
            mask >>= 1;
            vis = 0;
            return strategize();
        }
        // drain the next index
        assert(idx & ~vis);
        query = __builtin_ctz(idx & ~vis);
        check();
    }

    void strategize() {
        strategy = mask_strategy[mask];
        if (strategy == PICK) {
            pick();
        }
        if (strategy == DRAIN_ONE) {
            query = __builtin_ctz(idx);
        }
        if (strategy == DRAIN_ALL) {
            query = __builtin_ctz(idx);
        }
        check();
    }

    void check() {
        assert(strategy == PICK || CONTAINS(idx, query));
        assert(strategy != NONE);
        assert((vis & ~idx) == 0);
        assert((1 << ink) < mask);
        assert(__builtin_popcount(idx >= 2));
    }

    void dump_driver() {
        fprintf(stderr, "driver %d busted in round %d\n", t, judge_round);
        fprintf(stderr, "strategy: %d\n", strategy);
        fprintf(stderr, "mask:  %s\n", bits(mask).c_str());
        fprintf(stderr, "idx:   %s\n", bits(idx).c_str());
        fprintf(stderr, "vis:   %s\n", bits(vis).c_str());
        fprintf(stderr, "query: %d\n", query);
        fprintf(stderr, "   qs: %s\n", bits(1 << query).c_str());
        fprintf(stderr, "ink:   %d\n", ink);
    }
};

Driver drivers[MAXT];

void solve() {
    for (int t = 0; t < T; t++) {
        drivers[t] = Driver(t, N);
    }
    while (true) {
        string query_str;
        for (int t = 0; t < T; t++) {
            query_str.append(t > 0, ' ');
            query_str += to_string(drivers[t].query + 1);
        }
        cout << query_str << endl;

        if (drivers_done == T) {
            break;
        }

        judge_round++;
        assert(judge_round <= N * (N + 1) / 2);

        for (int t = 0; t < T; t++) {
            cin >> drivers[t].answer;
            drivers[t].update();
        }
    }

    string guess_str;
    for (int t = 0; t < T; t++) {
        guess_str.append(t > 0, ' ');
        guess_str += to_string(drivers[t].chosen[0] + 1);
        guess_str.append(1, ' ');
        guess_str += to_string(drivers[t].chosen[1] + 1);
    }
    cout << guess_str << endl;
}

// *****

int main() {
    cin >> T >> N >> C;
    prepare();
    int start = (1 << N) - 1;
    fprintf(stderr, "optimal: %lf\n", mask_optimal[start]);
    fprintf(stderr, "initial strategy: %d\n", mask_strategy[start]);
    solve();
    return 0;
}
