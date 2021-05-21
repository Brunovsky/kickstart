#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXS 100'001
#define BADV 3'000'010

int S;
int M[MAXS];
int N[MAXS];
int cnts[MAXS];
map<int, int> uniq_cnt;     // how many times N appears as the only mapped value
map<int, multiset<int>> Ns; // M corresponds with these Ns
int max_uniques;            // maximum number of unique Ns among all Ms

void remove_uniq(int n) {
    int c = uniq_cnt[n];
    cnts[c]--;
    cnts[c - 1]++;
    uniq_cnt[n]--;
    if (cnts[max_uniques] == 0) {
        max_uniques--;
    }
}

void insert_uniq(int n) {
    int c = uniq_cnt[n];
    cnts[c]--;
    cnts[c + 1]++;
    uniq_cnt[n]++;
    if (cnts[max_uniques + 1] != 0) {
        max_uniques++;
    }
}

#define UNIQUE(mset) (!mset.empty() && *mset.begin() == *mset.rbegin())

void remove_window(int i) {
    int m = M[i], n = N[i];
    if (UNIQUE(Ns[m])) {
        int mapped = *Ns[m].begin();
        remove_uniq(mapped);
    }
    Ns[m].erase(Ns[m].find(n));
    if (UNIQUE(Ns[m])) {
        int mapped = *Ns[m].begin();
        insert_uniq(mapped);
    }
    if (Ns[m].empty()) {
        Ns.erase(m);
    }
}

void insert_window(int i) {
    int m = M[i], n = N[i];
    if (UNIQUE(Ns[m])) {
        int mapped = *Ns[m].begin();
        remove_uniq(mapped);
    }
    Ns[m].insert(n);
    if (UNIQUE(Ns[m])) {
        int mapped = *Ns[m].begin();
        insert_uniq(mapped);
    }
}

bool ok() {
    // max_uniques == numMs or numMs - 1
    int numMs = Ns.size();
    return max_uniques + 1 >= numMs;
}

auto solve() {
    memset(cnts, 0, sizeof(cnts));
    uniq_cnt.clear(), Ns.clear();
    max_uniques = 0;
    cin >> S;
    for (int i = 0; i < S; i++) {
        int D, A, B;
        cin >> D >> A >> B;
        M[i] = D + A;
        N[i] = D - B;
    }
    M[S] = N[S] = BADV;
    int best = 0, cnt_best = 0;
    int i = 0, j = 0;
    while (i < S) {
        while (j <= S && ok()) {
            insert_window(j++);
        }
        int size_window = j - i - 1;
        if (best < size_window) {
            best = size_window;
            cnt_best = 1;
        } else if (best == size_window) {
            cnt_best++;
        }
        remove_window(i++);
    }
    return to_string(best) + ' ' + to_string(cnt_best);
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
