#include <bits/stdc++.h>

using namespace std;

// *****

#define BLOCK         10'000'000'000L
#define NBLOCKS       100
#define WIN           -2
#define LOSS          -3
#define QUANTITY(run) ((run.R - run.L) / BLOCK)

struct run_t {
    long L, R;
};

bool operator<(run_t lhs, run_t rhs) {
    return make_pair(lhs.R - lhs.L, lhs.L) < make_pair(rhs.R - rhs.L, rhs.L);
}

set<run_t> runset;

auto contains(run_t range) {
    return [range](run_t run) {
        return run.L <= range.L && range.R <= run.R;
    };
}

void remove(long Q) {
    long L = Q, R = Q + BLOCK;
    auto run = *find_if(begin(runset), end(runset), contains({L, R}));
    assert(run.L <= L && R <= run.R);
    runset.erase(run);

    run_t left{run.L, L};
    run_t right{R, run.R};
    if (QUANTITY(left) >= 1) {
        runset.insert(left);
    }
    if (QUANTITY(right) >= 1) {
        runset.insert(right);
    }
}

void WRITE(long Q) {
    cout << Q << endl;
    remove(Q);
}

long READ() {
    long P;
    cin >> P;
    if (P == -1 || P < -3)
        exit(0); // die gracefully
    if (P >= 0)
        remove(P);
    return P;
}

run_t largest_run() {
    return *runset.rbegin();
}

// there are easier ways to do this
int single_runs() {
    auto it = runset.upper_bound({BLOCK * NBLOCKS, BLOCK * NBLOCKS + 2 * BLOCK - 1});
    return distance(begin(runset), it);
}

void solve() {
    runset.clear();
    runset.insert({1, BLOCK * NBLOCKS + 1});

    // create perfect runs for as long as it is possible to do so.
    while (true) {
        READ();
        run_t run = largest_run();
        if (QUANTITY(run) <= 2) {
            break; // phase 2
        }
        WRITE(run.L + 2 * BLOCK);
    }

    // cannot create any more perfect runs, have to make do with what we got.
    // N     left       middle
    // 2     L->L+1     L->L
    // 1     L->L       L->L-1
    // we want ->L' odd.
    while (true) {
        int L = single_runs();
        int N = runset.size() - L;
        run_t run = largest_run();

        if (N == 0 || N > 2) {
            WRITE(run.L);
        } else if (N == 2) {
            if (L & 1) {
                WRITE(run.L + BLOCK - 1);
            } else {
                WRITE(run.L);
            }
        } else {
            if (L & 1) {
                WRITE(run.L);
            } else {
                WRITE(run.L + BLOCK - 1);
            }
        }

        long P = READ();
        if (P == LOSS || P == WIN) {
            return;
        }
    }
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    unsigned T, W;
    cin >> T >> W >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
