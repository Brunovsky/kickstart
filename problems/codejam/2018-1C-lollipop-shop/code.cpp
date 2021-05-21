#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 201

int N;
int cnt[MAXN];
bool sold[MAXN];

void READ(vector<int>& flavors) {
    int D;
    cin >> D;
    assert(D != -1);
    flavors.resize(D);
    for (int i = 0; i < D; i++) {
        cin >> flavors[i];
        assert(flavors[i] != -1);
    }
}

void WRITE(int flavor) {
    cout << flavor << endl;
}

void solve() {
    memset(cnt, 0, sizeof(cnt));
    memset(sold, 0, sizeof(sold));
    cin >> N;
    cnt[N] = INT_MAX;
    vector<int> flavors;
    for (int i = 0; i < N; i++) {
        READ(flavors);
        int best = N;
        for (int f : flavors) {
            if (!sold[f] && cnt[f] < cnt[best]) {
                best = f;
            }
        }
        if (best != N) {
            WRITE(best);
            sold[best] = true;
        } else {
            WRITE(-1);
        }
        for (int f : flavors) {
            cnt[f]++;
        }
    }
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    unsigned T;
    cin >> T;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
