#include <bits/stdc++.h>

using namespace std;

// *****

struct State {
    int V;
    double N;
};

void CHECK_DAY(int day) {
    int _day;
    cin >> _day;
    assert(day == _day);
}

void WRITE(int V, int P) {
    cout << V << ' ' << P << endl;
}

void READ(int day, State& state) {
    cout << state.V << ' ' << 0 << endl;
    int N;
    cin >> N;
    // expected number of votes in the 100th day:
    state.N = N + (100 - day) * 0.05;
    for (int i = 0; i < N; i++) {
        int discard;
        cin >> discard;
    }
}

// Strat 1: 1350/2500
void brute() {
    for (int day = 1; day <= 99; day++) {
        CHECK_DAY(day);
        int V = (day % 19) + 1;
        WRITE(V, 100);
    }
    CHECK_DAY(100);
    WRITE(20, 100);
}

// Strat 2: 1850/2500
void brute_careful() {
    for (int day = 1; day <= 99; day++) {
        CHECK_DAY(day);
        int V = (day % 19) + 1;
        int P = day <= 19 ? 100 : day;
        WRITE(V, P);
    }
    CHECK_DAY(100);
    WRITE(20, 100);
}

bool less_players(const State& lhs, const State& rhs) {
    return make_tuple(lhs.N, lhs.V) < make_tuple(rhs.N, rhs.V);
}

void ssort(vector<State>& states) {
    sort(begin(states), end(states), less_players);
}

void split(vector<State>& good, vector<State>& bad) {
    int keep = (1 + good.size()) / 2;
    ssort(good);
    bad.insert(bad.end(), make_move_iterator(begin(good) + keep),
               make_move_iterator(end(good)));
    ssort(bad);
    good.resize(keep);
}

#define THRESHOLD 4.0

// Strat 3: 2350/2500
void splitter() {
    vector<State> good, bad;
    for (int v = 0; v < 20; v++) {
        good.push_back({v + 1, 0.0});
    }

    int day = 1;
    while (day < 100) {
        for (uint v = 0; v < good.size() && day < 100; v++) {
            CHECK_DAY(day);
            READ(day, good[v]);
            day++;
        }
        split(good, bad);
        if (good.size() == 1) {
            break;
        }
        while (bad[0].N - good[0].N <= THRESHOLD && day < 100) {
            CHECK_DAY(day);
            WRITE(bad[0].V, day);
            day++;
            bad[0].N++;
            ssort(bad);
        }
    }

    while (day < 100) {
        ssort(bad);
        CHECK_DAY(day);
        WRITE(bad[0].V, day);
        day++;
        bad[0].N++;
    }

    assert(!good.empty());
    ssort(good);
    CHECK_DAY(100);
    WRITE(good[0].V, 100);
}

void solve() {
    splitter();
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
