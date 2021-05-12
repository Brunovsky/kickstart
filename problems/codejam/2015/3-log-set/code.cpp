#include <bits/stdc++.h>

using namespace std;

// *****

using PowerSet = map<long, long>;

template <typename T>
optional<vector<int>> sparse_subset_sum(const vector<T>& nums, T target) {
    int N = nums.size();
    map<T, int> seen;
    seen[T(0)] = -1;

    for (int i = 0; i < N && !seen.count(target); i++) {
        int x = nums[i];
        if (x > 0) {
            for (auto it = seen.rbegin(); it != seen.rend(); ++it) {
                seen.emplace(it->first + x, i);
            }
        } else if (x < 0) {
            for (auto it = seen.begin(); it != seen.end(); ++it) {
                seen.emplace(it->first + x, i);
            }
        }
    }

    if (seen.count(target)) {
        vector<int> indices;
        while (target != 0) {
            int i = seen.at(target);
            indices.push_back(i);
            target -= nums[i];
        }
        reverse(begin(indices), end(indices));
        return indices;
    } else {
        return std::nullopt;
    }
}

void generate() {
    int N = 20;
    using intd = uniform_int_distribution<int>;
    mt19937 mt(random_device{}());

    vector<int> nums(N);
    for (int i = 0; i < N; i++) {
        nums[i] = intd(-5, 5)(mt);
    }
    sort(begin(nums), end(nums));

    PowerSet power;
    for (int i = 0; i < (1 << N); i++) {
        int sum = 0;
        for (int b = 0; b < N; b++) {
            if (i & (1 << b)) {
                sum += nums[b];
            }
        }
        power[sum]++;
    }
    int P = power.size();

    cout << N << ' ' << P << endl;
    for (auto [n, c] : power) {
        cout << n << ' ';
    }
    cout << endl;
    for (auto [n, c] : power) {
        cout << c << ' ';
    }
    cout << endl << "ANSWER: ";
    for (int n : nums) {
        cout << n << ' ';
    }
    cout << endl;
}

auto solve() {
    int P;
    cin >> P;

    long K = 0;
    vector<long> numbers(P);
    PowerSet power;

    for (int i = 0; i < P; i++) {
        cin >> numbers[i];
    }
    for (int i = 0; i < P; i++) {
        cin >> power[numbers[i]];
        K += power[numbers[i]];
    }

    vector<int> S;

    // starting by removing zeros from S
    long zeros = 0;
    if (power.begin()->first < 0) {
        while (power.begin()->second > (1LL << zeros)) {
            zeros++, S.push_back(0);
        }
    } else if (power.rbegin()->first > 0) {
        while (power.rbegin()->second > (1LL << zeros)) {
            zeros++, S.push_back(0);
        }
    } else {
        while (power.at(0) > (1LL << zeros)) {
            zeros++, S.push_back(0);
        }
    }
    for (auto& [n, c] : power) {
        c /= 1LL << zeros;
    }
    K /= 1LL << zeros;

    long maxvalue = power.rbegin()->first;

    // check if we can split the powerset on k; i.e. if S can contain k
    auto split = [&](long k) -> optional<PowerSet> {
        PowerSet remaining = power, split_power;
        assert(k != 0);
        while (!remaining.empty()) {
            auto [n, c] = k > 0 ? *remaining.begin() : *remaining.rbegin();
            assert(c > 0);
            if (remaining.count(n + k) && remaining.at(n + k) >= c) {
                split_power[n] += c;
                remaining.erase(n);
                remaining[n + k] -= c;
                if (remaining[n + k] == 0) {
                    remaining.erase(n + k);
                }
            } else {
                return std::nullopt;
            }
        }

        return split_power;
    };

    vector<int> diffs;

    while (K > 1) {
        int M = power.size();
        assert(M >= 2);

        long max_sum = power.rbegin()->first;
        long max2_sum = next(power.rbegin())->first;
        long diff = max_sum - max2_sum;
        assert(diff > 0);

        auto split_pos_opt = split(diff);
        assert(split_pos_opt.has_value());

        diffs.push_back(diff);
        power = *split_pos_opt;
        K >>= 1;
    }

    assert(K == 1);
    assert(power.size() == 1u);

    auto indices_opt = sparse_subset_sum(diffs, maxvalue);
    assert(indices_opt.has_value());
    auto indices = move(*indices_opt);

    int D = diffs.size();
    vector<bool> positive(D);
    for (int i : indices) {
        positive[i] = true;
    }
    for (int i = 0; i < D; i++) {
        S.push_back(positive[i] ? diffs[i] : -diffs[i]);
    }

    sort(begin(S), end(S));
    int N = S.size();
    for (int i = 0; i < N; i++) {
        cout << " " << S[i];
    }
}

// *****

int main() {
    // return generate(), 0;
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
        cout << endl;
    }
    return 0;
}
