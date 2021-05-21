#include <bits/stdc++.h>

using namespace std;

// *****

struct Interval {
    long L;
    double R;
};

bool operator<(Interval lhs, Interval rhs) {
    return make_pair(lhs.L, lhs.R) < make_pair(rhs.L, rhs.R);
}

ostream& operator<<(ostream& out, Interval intv) {
    return out << '[' << intv.L << ',' << intv.R << ']';
}

#define MAXN 100

int N;
long P;
int W[MAXN];
int H[MAXN];
Interval cut[MAXN];

// This is greedy O(N) in the number of intervals. The thing you must realize is that
// the intervals are all quite large (at least 2min(W[i], H[i])>=2), so the set 'all'
// expands considerably with every insertion, and lots of intervals get merged.
// That's why the size of the interval remains ~O(N) and not O(2^N).
void insert(vector<Interval>& all, Interval intv) {
    for (uint i = 0, N = all.size(); i < N; i++) {
        all.push_back({all[i].L + intv.L, all[i].R + intv.R});
    }
    sort(begin(all), end(all));
    uint read = 1, write = 0, N = all.size();
    while (read < N) {
        if (all[read].L <= all[write].R) {
            all[write].R = max(all[read].R, all[write].R);
        } else {
            write++;
            all[write] = all[read];
        }
        read++;
    }
    all.resize(write + 1);
}

auto solve() {
    cin >> N >> P;
    long plain = 0;
    for (int i = 0; i < N; i++) {
        cin >> W[i] >> H[i];
        cut[i].L = 2 * min(W[i], H[i]);
        cut[i].R = 2 * sqrt(W[i] * W[i] + H[i] * H[i]);
        plain += 2 * (W[i] + H[i]);
    }
    if (plain == P) {
        return 1.0 * P;
    }
    long extra = P - plain;
    sort(cut, cut + N);
    vector<Interval> intervals{{0, 0}};
    for (int i = 0; i < N; i++) {
        insert(intervals, cut[i]);
    }
    double best = 0;
    for (uint i = 0; i < intervals.size(); i++) {
        if (intervals[i].L <= extra && extra <= intervals[i].R) {
            return 1.0 * P;
        } else if (intervals[i].R < extra) {
            best = intervals[i].R;
        } else if (extra < intervals[i].L) {
            break;
        }
    }
    return plain + best;
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(8);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
