#include <bits/stdc++.h>

using namespace std;

// *****

using Skillset = array<ushort, 5>;

struct hasher {
    inline size_t operator()(const Skillset &vec) const noexcept {
        size_t seed = vec.size();
        for (auto i : vec) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

unordered_map<Skillset, uint, hasher> skillsets;

auto subskillsets(const Skillset &skillset) {
    vector<Skillset> sub{{}};
    for (int i = 0, S = 1; i < 5 && skillset[i]; i++, S *= 2) {
        sub.resize(2 * S);
        int skill = skillset[i];
        for (int j = 0; j < S; j++) {
            sub[j + S] = sub[j];
            for (int k = 0; k < 5; k++) {
                if (sub[j][k] == 0) {
                    sub[j + S][k] = skill;
                    break;
                }
            }
        }
    }
    sub.pop_back();
    return sub;
}

auto solve() {
    ulong N, S;
    cin >> N >> S;
    skillsets.clear();

    for (ulong n = 0; n < N; ++n) {
        ulong C;
        cin >> C;

        Skillset skillset = {0};
        for (ushort c = 0; c < C; ++c) {
            cin >> skillset[c];
        }

        sort(begin(skillset), begin(skillset) + C);
        skillsets[skillset]++;
    }

    ulong exceptions = 0;

    for (const auto &entry : skillsets) {
        auto skillset = entry.first;
        uint cnt = entry.second;

        uint cannot_teach = 0;
        auto subsets = subskillsets(skillset);
        for (const auto &subset : subsets) {
            // careful, don't create new entries in the map
            cannot_teach += skillsets.count(subset) ? skillsets[subset] : 0;
        }
        exceptions += cnt * (cnt - 1 + cannot_teach);
    }

    return N * (N - 1) - exceptions;
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
