#include <bits/stdc++.h>

using namespace std;

// *****

struct disjoint_set {
    int N, S;
    vector<int> next, size;

    disjoint_set(int N) : N(N), S(N), next(N), size(N, 1) {
        iota(begin(next), end(next), 0);
    }

    bool same(int i, int j) { return find(i) == find(j); }

    int find(int i) {
        while (i != next[i]) {
            i = next[i] = next[next[i]];
        }
        return i;
    }

    void join(int i, int j) {
        i = find(i);
        j = find(j);
        if (i != j) {
            if (size[i] < size[j]) {
                swap(i, j);
            }
            next[j] = i;
            size[i] += size[j];
            S--;
        }
    }

    vector<vector<int>> group() {
        vector<vector<int>> groups(S);
        vector<int> mapping(N);
        int s = 0;
        for (int i = 0; i < N; i++) {
            if (i == find(i))
                mapping[i] = s++;
        }
        for (int i = 0; i < N; i++) {
            groups[mapping[find(i)]].push_back(i);
        }
        return groups;
    }
};

vector<int> adj;
vector<vector<int>> rev;

int brent(int& x, int& y) {
    int power = 1, len = 1;
    while (x != y) {
        if (power == len) {
            x = y;
            power <<= 1;
            len = 0;
        }
        y = adj[y];
        len++;
    }
    return len;
}

int dist(int x, int p) {
    int len = 0;
    for (int y : rev[x])
        if (y != p)
            len = max(len, 1 + dist(y, x));
    return len;
}

auto solve() {
    int N;
    cin >> N;
    adj.resize(N);
    rev.assign(N, {});
    disjoint_set set(N);

    for (int i = 0; i < N; i++) {
        cin >> adj[i];
        rev[--adj[i]].push_back(i);
        set.join(i, adj[i]);
    }

    int S = set.S;
    auto groups = set.group();

    int binary = 0;
    int single = 0;

    for (int i = 0; i < S; i++) {
        int x = groups[i][0];
        int y = adj[x];
        int len = brent(x, y);
        assert(groups[i].size() > 1U);

        if (len > 2) {
            single = max(len, single);
        } else {
            assert(len == 2);
            y = adj[x];
            binary += 2 + dist(x, y) + dist(y, x);
        }
    }

    return max(binary, single);
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
