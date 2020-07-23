#include <bits/stdc++.h>

using namespace std;

// *****

struct Slot {
    long coding, eating;
};

struct Day {
    long coding, eating;
    int i;
};

#define MAXS 100'000
#define MAXD 100'000

int D, S;
Slot slots[MAXS];
Day days[MAXD];

inline bool cmp_coding(const Day &lhs, const Day &rhs) {
    return lhs.coding < rhs.coding;
}

inline bool cmp_ratio(const Slot &s1, const Slot &s2) {
    return s1.coding * s2.eating > s1.eating * s2.coding;
}

auto solve() {
    cin >> D >> S >> ws;
    memset(slots, 0, sizeof(slots));
    memset(days, 0, sizeof(days));
    for (int s = 0; s < S; ++s) {
        cin >> slots[s].coding >> slots[s].eating >> ws;
    }
    for (int d = 0; d < D; ++d) {
        cin >> days[d].coding >> days[d].eating >> ws;
        days[d].i = d;
    }

    // Sort the days by coding requirement, increasing.
    // Sort the slots in such a way that the ratio coding/eating is decreasing.
    sort(days, days + D, cmp_coding);
    sort(slots, slots + S, cmp_ratio);

    long coding = 0, eating = 0;
    for (int s = 0; s < S; ++s) {
        eating += slots[s].eating;
    }

    string text(D, 'N');
    for (int d = 0, s = 0; d < D; d++) {
        // transform next slot fully into coding?
        while (s < S && coding + slots[s].coding <= days[d].coding) {
            coding += slots[s].coding;
            eating -= slots[s++].eating;
        }

        if (s == S) {
            if (coding < days[d].coding) {
                break;
            }
            bool verdict = eating >= days[d].eating;
            text[days[d].i] = verdict ? 'Y' : 'N';
            continue;
        }

        long dc = days[d].coding - coding;
        long de = eating - days[d].eating;
        bool verdict = dc * slots[s].eating <= de * slots[s].coding;
        text[days[d].i] = verdict ? 'Y' : 'N';
    }
    return text;
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
