#include <bits/stdc++.h>

using namespace std;

// *****

namespace std {

// TODO: replace Num
template <typename Num>
struct hash<vector<Num>> {
    inline size_t operator()(const vector<Num> &vec) const noexcept {
        size_t seed = distance(begin(vec), end(vec));
        for (auto i : vec) {
            seed ^= size_t(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

} // namespace std
