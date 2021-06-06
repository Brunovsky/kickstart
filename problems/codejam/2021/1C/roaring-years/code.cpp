#include <bits/stdc++.h>

using namespace std;

// *****

using vi = vector<int>;
using vl = vector<long>;

long pow10[19];

void setup() {
    pow10[0] = 1;
    for (int i = 1; i < 19; i++) {
        pow10[i] = 10 * pow10[i - 1];
    }
}

vi repeat(int n, int k) { return vi(n, k); }
vi operator+(vi a, vi b) { return a.insert(end(a), begin(b), end(b)), a; }
vl split(string s, vi lens) {
    int S = s.size(), L = lens.size();
    assert(S == accumulate(begin(lens), end(lens), 0));
    vl nums(L);
    for (int i = 0, o = 0; i < L; i++) {
        nums[i] = stoll(s.substr(o, lens[i])), o += lens[i];
    }
    return nums;
}
long make(vl nums, vi lens) {
    string s;
    int N = nums.size();
    for (int i = 0; i < N; i++) {
        string b = to_string(nums[i]);
        int B = b.size();
        assert(B == lens[i]);
        s += b;
    }
    try {
        return stoll(s);
    } catch (std::out_of_range& err) { return LONG_MAX; }
}

auto find(long Y) {
    string s = to_string(Y);
    int S = s.size();

    long earliest = LONG_MAX;
    // try to break s into exactly n blocks
    for (int n = 2; n <= S; n++) {
        int m = S / n;
        auto lens = repeat(n - S % n, m) + repeat(S % n, m + 1);
        auto nums = split(s, lens);

        bool safe = false, ok = true;
        for (int i = 1; i < n; i++) {
            safe |= nums[i - 1] >= nums[i];
            ok &= safe || nums[i - 1] + 1 >= nums[i];
            nums[i] = nums[i - 1] + 1;
        }
        if (!ok) {
            for (int i = 0; i < n; i++) {
                nums[i]++;
            }
        }
        long delta = 0;
        for (int i = 0; i < n; i++) {
            delta = max(delta, pow10[lens[i] - 1] - nums[i]);
        }
        for (int i = 0; i < n; i++) {
            nums[i] += delta;
        }
        ok = true;
        for (int i = 0; ok && i < n; i++) {
            ok = pow10[lens[i] - 1] <= nums[i] && nums[i] < pow10[lens[i]];
        }
        if (ok) {
            earliest = min(earliest, make(nums, lens));
        }
    }
    return earliest;
}

void brute() {
    long MAX = 1234567;
    vector<bool> actual(MAX + 1, false);

    for (long y = 1; y <= MAX; y++) {
        string s = to_string(y);
        int S = s.size();
        for (int n = 2; !actual[y] && n <= S; n++) {
            int m = S / n;
            auto lens = repeat(n - S % n, m) + repeat(S % n, m + 1);
            auto nums = split(s, lens);

            bool is = true;
            for (int i = 1; is && i < n; i++) {
                is = pow10[lens[i] - 1] <= nums[i] && nums[i] < pow10[lens[i]] &&
                     nums[i] == nums[i - 1] + 1;
            }
            actual[y] = is;
        }
    }

    vector<long> earliest(MAX + 1);
    long cur = MAX;
    for (long y = MAX; y >= 1; y--) {
        if (actual[y]) {
            cur = y;
        }
        earliest[y] = cur;
    }

    int errors = 0;

    for (long y = 1; errors < 10 && y <= 1000000; y++) {
        int S = to_string(y).size();
        long got = min(find(y), find(pow10[S]));
        if (got != earliest[y]) {
            printf("=== %ld\nexpected: %ld\ngot: %ld\n", y, earliest[y], got);
            errors++;
        }
    }

    if (errors == 0)
        printf("OK\n");
}

auto solve() {
    long Y;
    cin >> Y, Y++;
    int S = to_string(Y).size();

    if (S > 18) {
        return find(Y);
    } else {
        return min(find(Y), find(pow10[S]));
    }
}

// *****

int main() {
    setup();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
