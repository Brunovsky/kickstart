#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<int> sticks(N);
    vector<int> counts;
    for (int i = 0; i < N; ++i) {
        cin >> sticks[i];
    }
    {
        sort(sticks.begin(), sticks.end());
        int write = 0, read = 0;
        while (read < N) {
            int end = read + 1;
            while (end < N && sticks[read] == sticks[end])
                ++end;
            counts.push_back(end - read);
            sticks[write++] = sticks[read];
            read = end;
        }
        sticks.resize(write);
        N = write;
    }
    uint64_t sets = 0;
    for (int s = 0; s < N; ++s) {
        if (counts[s] < 2)
            continue;
        uint64_t choose2 = (counts[s] * (counts[s] - 1)) / 2;
        uint64_t choose3 = (choose2 * (counts[s] - 2)) / 3;
        int i = 0; // shorter, inclusive
        int j = 1; // longer, exclusive
        int long_count = 0;
        do {
            while (j < N && sticks[j] - sticks[i] < 2 * sticks[s])
                long_count += counts[j++];
            bool is_short = i == s;
            bool is_long = i < s && s < j;
            if (is_short) {
                sets += choose3 * long_count;
            } else if (is_long) {
                sets += choose2 * counts[i] * (long_count - counts[s]);
                sets += choose3 * counts[i];
            } else {
                sets += choose2 * counts[i] * long_count;
            }
            long_count -= counts[++i];
        } while (i + 1 < N);
    }

    return sets;
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
