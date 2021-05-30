#include <bits/stdc++.h>

using namespace std;

// *****

template <typename T>
struct fenwick2d {
    int N, M;
    vector<vector<T>> tree;

    explicit fenwick2d(int N = 0, int M = 0)
        : N(N), M(M), tree(N + 1, vector<int>(M + 1)) {}

    T sum(int i, int j) const {
        T sum = 0;
        while (i > 0) {
            int k = j;
            while (k > 0) {
                sum += tree[i][k];
                k -= k & -k;
            }
            i -= i & -i;
        }
        return sum;
    }

    T rectangle(int i0, int j0, int i1, int j1) const {
        return sum(i1, j1) - sum(i0, j1) - sum(i1, j0) + sum(i0, j0);
    }

    void add(int i, int j, T n) {
        if (i > 0 && j > 0) {
            while (i <= N) {
                int k = j;
                while (k <= M) {
                    tree[i][k] += n;
                    k += k & -k;
                }
                i += i & -i;
            }
        }
    }
};

int A[800][800];
int id[800][800];

auto solve() {
    int N, K;
    cin >> N >> K;
    vector<int> values;
    values.reserve(N * N);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> A[i][j];
            values.push_back(A[i][j]);
        }
    }

    sort(begin(values), end(values));
    values.erase(unique(begin(values), end(values)), end(values));
    int V = values.size();

    unordered_map<int, int> idmap;
    for (int i = 0; i < V; i++) {
        idmap[values[i]] = i;
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            id[i][j] = idmap[A[i][j]];
        }
    }

    int index = (K * K + 1) / 2;

    int L = -1, R = V;
    while (L + 1 < R) {
        int M = (L + R) / 2;

        fenwick2d<int> tree(N, N);

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (id[i][j] <= M) {
                    tree.add(i + 1, j + 1, 1);
                }
            }
        }

        bool found = false;
        for (int i = K; i <= N && !found; i++) {
            for (int j = K; j <= N && !found; j++) {
                found = tree.rectangle(i - K, j - K, i, j) >= index;
            }
        }

        if (found) {
            R = M;
        } else {
            L = M;
        }
    }

    return values[R];
}

// *****

int main() {
    ios::sync_with_stdio(false);
    cout << solve() << endl;
    return 0;
}
