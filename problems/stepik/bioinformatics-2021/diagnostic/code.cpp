#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

string align_string_matrix(const vector<vector<string>>& mat) {
    vector<size_t> width;
    for (int i = 0, N = mat.size(); i < N; i++) {
        width.resize(max(width.size(), mat[i].size()));
        for (int j = 0, M = mat[i].size(); j < M; j++) {
            width[j] = max(width[j], mat[i][j].size());
        }
    }
    string s;
    for (int i = 0, N = mat.size(); i < N; i++) {
        for (int j = 0, M = mat[i].size(); j < M; j++) {
            string pad(width[j] - mat[i][j].size(), ' ');
            s += pad + mat[i][j] + " \n"[j + 1 == M];
        }
    }
    return s;
}

template <typename U>
string format_histogram(const map<U, int>& hist) {
    long sum = 0;
    int max_frequency = 0;
    for (const auto& [key, frequency] : hist) {
        sum += frequency;
        max_frequency = max(max_frequency, frequency);
    }
    assert(max_frequency > 0);
    constexpr int BARS = 60;
    vector<vector<string>> table;
    for (const auto& [key, frequency] : hist) {
        int length = llround(1.0 * frequency / max_frequency * BARS);
        string bars = string(length, '#') + string(BARS - length, ' ');
        table.push_back({to_string(key), to_string(frequency), bars});
    }
    return align_string_matrix(table);
}

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

struct lca_schieber_vishkin {
    int N, timer = 0;
    vector<int> preorder, up, I, A, head, depth;

    static int lowest_one_bit(int n) { return n & -n; }
    static int highest_one_bit(int n) { return n ? 1 << (31 - __builtin_clz(n)) : 0; }

    explicit lca_schieber_vishkin(const vector<vector<int>>& tree, int root, int zero = 0)
        : N(tree.size()), preorder(N), up(N), I(N), A(N), head(N), depth(N) {
        init_dfs1(tree, root, zero);
        init_dfs2(tree, root, zero, 0);
    }

    void init_dfs1(const vector<vector<int>>& tree, int u, int p) {
        up[u] = p;
        I[u] = preorder[u] = timer++;
        for (int v : tree[u]) {
            if (v != p) {
                depth[v] = depth[u] + 1;
                init_dfs1(tree, v, u);
                if (lowest_one_bit(I[u]) < lowest_one_bit(I[v])) {
                    I[u] = I[v];
                }
            }
        }
        head[I[u]] = u;
    }

    void init_dfs2(const vector<vector<int>>& tree, int u, int p, int up) {
        A[u] = up | lowest_one_bit(I[u]);
        for (int v : tree[u]) {
            if (v != p) {
                init_dfs2(tree, v, u, A[u]);
            }
        }
    }

    int parent(int u) const { return up[u]; }

    int enter_into_strip(int u, int hz) const {
        if (lowest_one_bit(I[u]) == hz)
            return u;
        int hw = highest_one_bit(A[u] & (hz - 1));
        return parent(head[(I[u] & -hw) | hw]);
    }

    int lca(int u, int v) const {
        int hb = I[u] == I[v] ? lowest_one_bit(I[u]) : highest_one_bit(I[u] ^ I[v]);
        int hz = lowest_one_bit(A[u] & A[v] & -hb);
        int eu = enter_into_strip(u, hz);
        int ev = enter_into_strip(v, hz);
        return preorder[eu] < preorder[ev] ? eu : ev;
    }

    int dist(int u, int v) const { return depth[u] + depth[v] - 2 * depth[lca(u, v)]; }
};

void progress_bar(int i, int N) {
    fprintf(stderr, "\rprogress %5.1lf  %6d/%6d", 100.0 * (i + 1) / N, i + 1, N);
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);

    int N;
    cin >> N;
    vector<int> parent(N + 1);
    vector<vector<int>> tree(N + 1);
    for (int i = 2; i <= N; i++) {
        cin >> parent[i];
        tree[parent[i]].push_back(i);
    }
    vector<int> val(N + 1);
    for (int i = 1; i <= N; i++) {
        cin >> val[i];
    }
    lca_schieber_vishkin sv(tree, 1, 0);

    int M;
    cin >> M;
    int num_disease_nodes = 0;
    vector<vector<int>> disease(M);
    vector<vector<int>> diseases_here(N + 1);
    for (int d = 0; d < M; d++) {
        int size;
        cin >> size;
        disease[d].resize(size);
        for (int i = 0; i < size; i++) {
            cin >> disease[d][i];
            diseases_here[disease[d][i]].push_back(d);
        }
        num_disease_nodes += size;
    }

    int P;
    cin >> P;
    int num_patient_nodes = 0;
    vector<vector<int>> patient(P);
    vector<vector<int>> patients_here(N + 1);
    for (int n = 0; n < P; n++) {
        int size;
        cin >> size;
        patient[n].resize(size);
        for (int i = 0; i < size; i++) {
            cin >> patient[n][i];
            patients_here[patient[n][i]].push_back(n);
        }
        num_patient_nodes += size;
    }

    vector<int> depth(N + 1);
    vector<int> deepest(N + 1);
    vector<int> subsize(N + 1);
    vector<int> tin(N + 1), tout(N + 1);
    int dfs_timer = 0;

    y_combinator([&](auto self, int u, int p) -> void {
        subsize[u] = 1;
        deepest[u] = depth[u];
        tin[u] = dfs_timer++;
        for (int v : tree[u]) {
            if (v != p) {
                depth[v] = depth[u] + 1;
                self(v, u);
                subsize[u] += subsize[v];
                deepest[u] = max(deepest[u], deepest[v]);
            }
        }
        tout[u] = dfs_timer;
    })(1, 0);

    if (num_patient_nodes == P && num_disease_nodes == M) {
        vector<int> any_below(N + 1, -1);

        y_combinator([&](auto self, int u, int p) -> void {
            if (!diseases_here[u].empty()) {
                any_below[u] = diseases_here[u][0] + 1;
            }
            for (int v : tree[u]) {
                if (v != p) {
                    self(v, u);
                    any_below[u] = max(any_below[u], any_below[v]);
                }
            }
        })(1, 0);

        vector<int> answer_here(N + 1);

        y_combinator([&](auto self, int u, int p) -> void {
            if (any_below[u] != -1) {
                answer_here[u] = any_below[u];
            } else {
                answer_here[u] = answer_here[p];
            }
            for (int v : tree[u]) {
                if (v != p) {
                    self(v, u);
                }
            }
        })(1, 0);

        for (int n = 0; n < P; n++) {
            cout << answer_here[patient[n][0]] << '\n';
        }

        return 0;
    }

    for (int n = 0; n < P; n++) {
        progress_bar(n, P);
        int best_disease = 0;
        long best_sum = 0;

        long maximum_sum = 0;
        for (int i : patient[n]) {
            maximum_sum += val[i];
        }

        for (int d = 0; d < M && best_sum < maximum_sum; d++) {
            long disease_sum = 0;
            for (int i : patient[n]) {
                int largest = 0;
                for (int j : disease[d]) {
                    largest = max(largest, val[sv.lca(i, j)]);
                }
                disease_sum += largest;
            }
            if (best_sum < disease_sum) {
                best_disease = d;
                best_sum = disease_sum;
            }
        }
        cout << best_disease + 1 << '\n';
    }

    return 0;
}
