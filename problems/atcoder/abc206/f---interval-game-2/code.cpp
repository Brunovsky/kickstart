#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

struct Hasher {
    template <typename Container>
    size_t operator()(const Container& vec) const noexcept {
        using std::hash;
        hash<typename Container::value_type> hasher;
        size_t seed = distance(begin(vec), end(vec));
        for (const auto& n : vec) {
            seed ^= hasher(n) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
    template <typename U, typename V>
    size_t operator()(const pair<U, V>& p) const noexcept {
        using std::hash;
        size_t a = hash<U>{}(p.first), b = hash<V>{}(p.second);
        return (a + b) * (a + b + 1) / 2 + b;
    }
    template <typename U>
    size_t operator()(const array<U, 2>& p) const noexcept {
        using std::hash;
        hash<U> hasher;
        size_t a = hasher(p[0]), b = hasher(p[1]);
        return (a + b) * (a + b + 1) / 2 + b;
    }
    template <int i, typename Tuple>
    size_t tuple_compute(const Tuple& tuple) const noexcept {
        if constexpr (i == std::tuple_size_v<Tuple>) {
            return std::tuple_size_v<Tuple>;
        } else {
            using std::hash;
            hash<std::tuple_element_t<i, Tuple>> hasher;
            size_t seed = tuple_compute<i + 1, Tuple>(tuple);
            size_t h = hasher(std::get<i>(tuple));
            return seed ^ (h + 0x9e3779b9 + (seed << 6) + (seed >> 2));
        }
    }
    template <typename... Ts>
    size_t operator()(const tuple<Ts...>& t) const noexcept {
        return tuple_compute<0, tuple<Ts...>>(t);
    }
};

namespace std {

template <typename T, size_t N>
struct hash<array<T, N>> : Hasher {};
template <typename T>
struct hash<vector<T>> : Hasher {};
template <typename U, typename V>
struct hash<pair<U, V>> : Hasher {};
template <typename... Ts>
struct hash<tuple<Ts...>> : Hasher {};

} // namespace std

template <typename Fun>
class y_combinator_result {
    Fun fun_;

  public:
    template <typename T>
    explicit y_combinator_result(T&& fun) : fun_(std::forward<T>(fun)) {}

    template <typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return fun_(std::ref(*this), std::forward<Args>(args)...);
    }
};

template <typename Fun>
auto y_combinator(Fun&& fun) {
    return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

struct Int128Hasher {
    size_t operator()(__uint128_t x) const noexcept {
        array<uint64_t, 2>* arr = reinterpret_cast<array<uint64_t, 2>*>(&x);
        return Hasher{}(*arr);
    }
    size_t operator()(__int128_t x) const noexcept {
        array<uint64_t, 2>* arr = reinterpret_cast<array<uint64_t, 2>*>(&x);
        return Hasher{}(*arr);
    }
};

#ifdef LOCAL
namespace std {

template <>
struct hash<__int128_t> : Int128Hasher {};
template <>
struct hash<__uint128_t> : Int128Hasher {};

} // namespace std
#endif

int mex(const vector<int>& nums) {
    int M = nums.size();
    vector<bool> seen(M);
    for (int n : nums) {
        if (n < M) {
            seen[n] = true;
        }
    }
    for (int i = 0; i < M; i++) {
        if (!seen[i]) {
            return i;
        }
    }
    return M;
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);

    int MAXN = 101;
    vector<vector<int>> nimsum(MAXN, vector<int>(MAXN));

    for (int a = 0; a < MAXN; a++) {
        for (int b = a; b < MAXN; b++) {
            vector<int> nums;
            for (int al = 0; al < a; al++) {
                nums.push_back(al ^ b);
            }
            for (int bl = 0; bl < b; bl++) {
                nums.push_back(a ^ bl);
            }
            nimsum[a][b] = nimsum[b][a] = mex(nums);
        }
    }

    int T;
    cin >> T;
    while (T--) {
        int N;
        cin >> N;
        vector<array<short, 2>> interval(N);
        for (auto& [l, r] : interval) {
            cin >> l >> r;
        }
        unordered_map<__int128_t, int> grundy;
        __int128_t all = 0;
        for (int i = 0; i < N; i++) {
            all |= __int128_t(1) << i;
        }

        auto ans = y_combinator([&](auto self, __int128_t idmask) -> int {
            if (idmask == 0) {
                return 0;
            }
            if (grundy.count(idmask)) {
                return grundy.at(idmask);
            }
            vector<int> ids;
            for (int i = 0; i < N; i++) {
                if ((idmask >> i) & 1) {
                    ids.push_back(i);
                }
            }
            int R = ids.size();
            __int128_t nimmex = 0;
            for (int i = 0; i < R; i++) {
                auto [a, b] = interval[ids[i]];
                __int128_t left = 0, right = 0;
                for (int j = 0; j < R; j++) {
                    auto [c, d] = interval[ids[j]];
                    if (i != j && d <= a) {
                        left |= __int128_t(1) << ids[j];
                    }
                    if (i != j && b <= c) {
                        right |= __int128_t(1) << ids[j];
                    }
                }
                nimmex |= __int128_t(1) << nimsum[self(left)][self(right)];
            }
            for (int i = 0; i <= N; i++) {
                if (((nimmex >> i) & 1) == 0) {
                    grundy[idmask] = i;
                    return i;
                }
            }
            __builtin_unreachable();
        })(all);

        if (ans == 0) {
            cout << "Bob\n";
        } else {
            cout << "Alice\n";
        }
    }
    return 0;
}
