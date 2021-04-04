#include <bits/stdc++.h>

using namespace std;

// *****

struct hasher {
    template <typename Container>
    size_t operator()(const Container& vec) const noexcept {
        using std::hash;
        using T = typename Container::value_type;
        hash<T> hasher;
        size_t seed = distance(begin(vec), end(vec));
        for (auto n : vec) {
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
};

namespace std {

template <typename U, typename V>
struct hash<pair<U, V>> {
    size_t operator()(const pair<U, V>& uv) const { return hasher{}(uv); }
};
template <typename T, size_t N>
struct hash<array<T, N>> {
    size_t operator()(const array<T, N>& arr) const { return hasher{}(arr); }
};
template <typename T>
struct hash<vector<T>> {
    size_t operator()(const vector<T>& arr) const { return hasher{}(arr); }
};

} // namespace std

using int2 = array<int, 2>;
using vi = vector<int>;
using vii = pair<vector<int>, int>;

int N, C;
vector<vi> gu, gv;
vector<bool> visu, visv;
vector<int2> idsize;

void dfsu(int u, int n) {
    visu[u] = true, idsize[n][0]++;
    for (int v : gu[u]) {
        if (!visv[v]) {
            visv[v] = true, idsize[n][1]++;
            for (int w : gv[v])
                if (!visu[w])
                    dfsu(w, n);
        }
    }
}

auto solve() {
    int E = 0;
    cin >> N, C = 0;
    gu.assign(N, {});
    gv.assign(N, {});
    visu.assign(N, 0);
    visv.assign(N, 0);
    idsize.clear();

    for (int u = 0; u < N; u++) {
        string line;
        cin >> line;
        for (int v = 0; v < N; v++) {
            if (line[v] == '1')
                E++, gu[u].push_back(v), gv[v].push_back(u);
        }
    }

    for (int u = 0; u < N; u++)
        if (!visu[u])
            idsize.push_back({0, 0}), dfsu(u, C++);
    for (int v = 0; v < N; v++)
        if (!visv[v])
            idsize.push_back({0, 1}), visv[v] = C++;

    unordered_map<int2, int> by_topo;
    for (auto component : idsize)
        by_topo[component]++;

    int M = by_topo.size();
    vector<int2> sizes;
    vector<int> cnts;
    vector<int> nullset(M, 0);
    for (auto [size, cnt] : by_topo)
        sizes.push_back(size), cnts.push_back(cnt);

    assert(accumulate(begin(cnts), end(cnts), 0) == C);

    auto subset_size = [&sizes, &cnts, M](const vi& S) {
        int2 s = {0, 0};
        for (int i = 0; i < M; i++)
            s[0] += sizes[i][0] * S[i], s[1] += sizes[i][1] * S[i];
        return s;
    };

    unordered_map<vii, int> dp_prev, dp_next;
    dp_next[{nullset, 0}] = 0;

    for (int c = 1; c <= C; c++) {
        swap(dp_prev, dp_next);
        dp_next.clear();

        for (const auto& [prev, cost] : dp_prev) {
            auto [S, t] = prev;
            auto [U, V] = subset_size(S);

            for (int i = 0; i < M; i++) {
                if (S[i] == cnts[i])
                    continue;

                auto S_next = S;
                S_next[i]++;

                auto [u, v] = sizes[i];
                int t_next = U + u == V + v ? U + u : t;
                int cost_next = (t_next - t) * (t_next - t) + cost;

                vii next = {S_next, t_next};

                if (!dp_next.count(next)) {
                    dp_next[next] = cost_next;
                } else {
                    dp_next[next] = min(dp_next[next], cost_next);
                }
            }
        }
    }

    return dp_next.at({cnts, N}) - E;
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
