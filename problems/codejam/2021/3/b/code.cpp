#include <bits/stdc++.h>

using namespace std;

inline namespace lib {

template <typename Seq>
string seq_to_string(const Seq& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}

template <typename Mat>
string mat_to_string(const Mat& v) {
    int N = v.size();
    vector<vector<string>> cell(1);
    vector<size_t> width(1);
    for (const auto& row : v) {
        int M = row.size(), i = cell.size();
        cell.emplace_back(M + 1, "");
        width.resize(max(int(width.size()), M + 1), 0);
        int j = 1;
        for (const auto& col : row) {
            if constexpr (std::is_same<decltype(col), const string&>::value) {
                cell[i][j] = col;
            } else {
                cell[i][j] = to_string(col);
            }
            width[j] = max(width[j], cell[i][j].size());
            j++;
        }
    }
    int M = width.size() - 1;
    cell[0].resize(M + 1);
    for (int i = 1; i <= N; i++) {
        cell[i][0] = to_string(i - 1);
        width[0] = max(width[0], cell[i][0].size());
    }
    for (int j = 1; j <= M; j++) {
        cell[0][j] = to_string(j - 1);
        width[j] = max(width[j], cell[0][j].size());
    }
    string s;
    for (int i = 0; i <= N; i++) {
        for (int S = cell[i].size(), j = 0; j < S; j++) {
            s += string(width[j] + 1 - cell[i][j].size(), ' ') + cell[i][j];
        }
        s += '\n';
    }
    return s;
}

} // namespace lib

namespace std {

template <typename U, typename V>
string to_string(const pair<U, V>& uv) {
    return '(' + to_string(uv.first) + ',' + to_string(uv.second) + ')';
}
template <typename U, typename V>
ostream& operator<<(ostream& out, const pair<U, V>& v) {
    return out << to_string(v);
}

template <typename T>
string to_string(const array<T, 2>& uv) {
    return '(' + to_string(uv[0]) + ',' + to_string(uv[1]) + ')';
}
template <typename T, size_t N>
ostream& operator<<(ostream& out, const array<T, N>& v) {
    return out << to_string(v);
}

template <typename T, typename... Rs>
string to_string(const vector<T, Rs...>& v) {
    string s;
    for (const auto& el : v)
        s += to_string(el) + " ";
    return s.empty() ? s : (s.pop_back(), s);
}
template <typename T, typename... Rs>
ostream& operator<<(ostream& out, const vector<T, Rs...>& v) {
    return out << to_string(v);
}

} // namespace std

inline namespace lib {

template <typename... Ts>
void debugger(string_view vars, Ts&&... args) {
    cout << ">> [" << vars << "]: ";
    const char* delim = "";
    (..., (cout << delim << args, delim = ", "));
    cout << endl;
}
#define debug(...) debugger(#__VA_ARGS__, __VA_ARGS__)

} // namespace lib

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

template <typename Flow = long, typename FlowSum = Flow>
struct tidal_flow {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit tidal_flow(int V) : V(V), res(V) {}

    void add(int u, int v, Flow capacity, bool bothways = false) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0});
        res[v].push_back(E++), edge.push_back({{v, u}, bothways ? capacity : 0, 0});
    }

    vector<int> level, edges, Q;
    vector<Flow> p;
    vector<FlowSum> h, l;
    static constexpr FlowSum flowsuminf = numeric_limits<FlowSum>::max() / 2;

    bool bfs(int s, int t) {
        level.assign(V, -1);
        edges.clear();
        level[s] = 0;
        Q[0] = s;
        int i = 0, S = 1;
        while (i < S && level[Q[i]] != level[t]) {
            int u = Q[i++];
            for (int e : res[u]) {
                int v = edge[e].node[1];
                if (edge[e].flow < edge[e].cap) {
                    if (level[v] == -1) {
                        level[v] = level[u] + 1;
                        Q[S++] = v;
                    }
                    if (level[v] == level[u] + 1) {
                        edges.push_back(e);
                    }
                }
            }
        }
        return level[t] != -1;
    }

    FlowSum tide(int s, int t) {
        fill(begin(h), end(h), 0);
        h[s] = flowsuminf;
        for (int e : edges) {
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(edge[e].cap - edge[e].flow), h[w]);
            h[v] = h[v] + p[e];
        }
        if (h[t] == 0) {
            return 0;
        }
        fill(begin(l), end(l), 0);
        l[t] = h[t];
        for (auto it = edges.rbegin(); it != edges.rend(); it++) {
            int e = *it;
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(p[e]), min(h[w] - l[w], l[v]));
            l[v] -= p[e];
            l[w] += p[e];
        } // 1=push phase, 0=recover phase
        fill(begin(h), end(h), 0);
        h[s] = l[s];
        for (auto e : edges) {
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(p[e]), h[w]);
            h[w] -= p[e];
            h[v] += p[e];
            edge[e].flow += p[e];
            edge[e ^ 1].flow -= p[e];
        }
        return h[t];
    }

    FlowSum maxflow(int s, int t) {
        h.assign(V, 0);
        l.assign(V, 0);
        p.assign(E, 0);
        Q.resize(V);
        FlowSum max_flow = 0, df;
        while (bfs(s, t)) {
            do {
                df = tide(s, t);
                max_flow += df;
            } while (df > 0);
        }
        return max_flow;
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

auto solve() {
    int R, C;
    cin >> R >> C;
    vector<int> S(R), T(C);
    for (int i = 0; i < R; i++) {
        cin >> S[i];
    }
    for (int i = 0; i < C; i++) {
        cin >> T[i];
    }
    int sum_row = accumulate(begin(S), end(S), 0);
    int sum_col = accumulate(begin(T), end(T), 0);
    if (sum_row != sum_col) {
        cout << "IMPOSSIBLE" << endl;
        return;
    }
    int s = R + C, t = R + C + 1;
    tidal_flow<int, int> mf(R + C + 2);
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            mf.add(r, c + R, 1); // edge r * C + c
        }
    }
    for (int i = 0; i < R; i++) {
        if (S[i] > 0)
            mf.add(s, i, S[i]);
    }
    for (int i = 0; i < C; i++) {
        if (T[i] > 0)
            mf.add(i + R, t, T[i]);
    }
    int maxflow = mf.maxflow(s, t);
    if (maxflow != sum_row) {
        cout << "IMPOSSIBLE" << endl;
        return;
    }
    vector<vector<int8_t>> state(R, vector<int8_t>(C, false));
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            state[r][c] = !!mf.get_flow(r * C + c);
        }
    }
    auto has_square = [&](int r, int c, int size) {
        bool is = true;
        for (int i = 0; is && i < size; i++) {
            is &= state[r + i][c - i];
            is &= !state[r + i][c + 1 + i];
            is &= state[r + i + size][c - i + size];
            is &= !state[r + i + size][c + 1 + i - size];
        }
        return is;
    };
    auto toggle = [&](int r, int c, int size) {
        for (int i = 0; i < size; i++) {
            state[r + i][c - i] ^= 1;
            state[r + i][c + 1 + i] ^= 1;
            state[r + i + size][c - i + size] ^= 1;
            state[r + i + size][c + 1 + i - size] ^= 1;
        }
    };
    bool ok = true;
    do {
        ok = true;
        for (int r = 0; r < R && ok; r++) {
            for (int c = 0; c < C && ok; c++) {
                int b0 = c + 1;
                int b1 = C - c - 1;
                int b2 = (R - r) / 2;
                int maxwidth = min(b0, min(b1, b2));
                for (int size = 1; size <= maxwidth && ok; size++) {
                    if (has_square(r, c, size)) {
                        ok = false;
                        toggle(r, c, size);
                    }
                }
            }
        }
    } while (!ok);
    cout << "POSSIBLE\n";
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            cout << (state[r][c] ? "/" : "\\") << (c + 1 == C ? "\n" : "");
        }
    }
}

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
    }
    return 0;
}
