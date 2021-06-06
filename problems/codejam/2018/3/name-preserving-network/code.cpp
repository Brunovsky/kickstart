#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXV 51

mt19937 mt(random_device{}());

void vector_erase(vector<int>& vec, int n) { vec.erase(find(begin(vec), end(vec), n)); }

struct vec_hasher {
    template <template <typename...> class Container, typename V, typename... T>
    size_t operator()(const Container<V, T...>& vec) const noexcept {
        hash<V> hasher;
        size_t seed = distance(begin(vec), end(vec));
        for (auto n : vec) {
            seed ^= hasher(n) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
} hasher;

struct graph {
    int V;
    bool mat[MAXV][MAXV];
    vector<vector<int>> adj;
    vector<size_t> vhash[3];
    unordered_map<size_t, int> hashmap[3];

    graph(int V) : V(V) {
        memset(mat, 0, sizeof(mat));
        adj.assign(V + 1, {});
        vhash[0].resize(V + 1);
        vhash[1].resize(V + 1);
        vhash[2].resize(V + 1);
    }

    void add(int u, int v) {
        assert(u != v && !mat[u][v] && !mat[v][u]);
        mat[u][v] = mat[v][u] = true;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    bool generate() {
        vector<int> vs(V);
        iota(begin(vs), end(vs), 1);

        while (!vs.empty()) {
            shuffle(begin(vs), end(vs), mt);
            int S = vs.size();

            int n = vs[0];
            int m = -1;
            for (int i = 1; i < S; i++) {
                if (!mat[n][vs[i]]) {
                    m = vs[i];
                    break;
                }
            }

            if (m == -1) {
                return false;
            }

            add(n, m);

            if (adj[n].size() == 4UL)
                vector_erase(vs, n);
            if (adj[m].size() == 4UL)
                vector_erase(vs, m);
        }

        vector<bool> vis(V + 1, false);
        queue<int> Q;
        Q.push(1);
        vis[1] = true;
        int S = 1;

        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            for (int v : adj[u]) {
                if (!vis[v]) {
                    Q.push(v);
                    vis[v] = true;
                    S++;
                }
            }
        }

        // completely connected
        return S == V;
    }

    size_t get_hash(int u) const { return vhash[2][u]; }

    int get_vertex(size_t hash) const { return hashmap[2].at(hash); }

    void hash_cnt(int i) {
        set<int> bfs{i};
        vector<int> cnts{1};
        int S = 1, iterations = V;

        while (iterations-- && S < V) {
            set<int> new_bfs;
            for (int u : bfs) {
                for (int v : adj[u]) {
                    new_bfs.insert(v);
                }
            }
            swap(bfs, new_bfs);
            S = bfs.size();
            cnts.push_back(S);
        }

        size_t hash = hasher(cnts);
        vhash[0][i] = hash;
        hashmap[0][hash] = i;
    }

    void hash_bfs(int i, int c) {
        set<int> bfs{i};
        vector<size_t> globs{vhash[c][i]};
        int S = 1, iterations = V;

        while (iterations-- && S < V) {
            set<int> new_bfs;
            vector<size_t> hashes;
            for (int u : bfs) {
                for (int v : adj[u]) {
                    if (!new_bfs.count(v)) {
                        new_bfs.insert(v);
                        hashes.push_back(vhash[c][v]);
                    }
                }
            }
            swap(bfs, new_bfs);
            S = bfs.size();
            sort(begin(hashes), end(hashes));
            globs.push_back(hasher(hashes));
        }

        size_t hash = hasher(globs);
        vhash[c + 1][i] = hash;
        hashmap[c + 1][hash] = i;
    }

    bool hash_graph() {
        for (int u = 1; u <= V; u++) {
            hash_cnt(u);
        }
        for (int c : {0, 1}) {
            for (int u = 1; u <= V; u++) {
                hash_bfs(u, c);
            }
        }

        // each vertex has a different hash
        int H = hashmap[2].size();
        return H == V;
    }
};

unordered_map<int, graph> graphs;

void setup() {
    for (int V = 10; V <= 50; V++) {
        int trials = 0;
        while (trials++ < 1000) {
            graph g(V);
            bool ok = g.generate() && g.hash_graph();
            if (ok) {
                graphs.emplace(V, g);
                break;
            }
        }
        assert(graphs.count(V));
        fprintf(stderr, "DONE %2d -- %d\n", V, trials);
    }
}

// *****

void WRITE_GRAPH(const graph& G) {
    stringstream ss;
    ss << G.V << endl;
    for (int u = 1; u < G.V; u++) {
        for (int v = u + 1; v <= G.V; v++) {
            if (G.mat[u][v]) {
                ss << u << ' ' << v << '\n';
            }
        }
    }
    cout << ss.str();
}

void WRITE_MAPPING(const vector<int>& mapping) {
    int V = mapping.size() - 1;
    stringstream ss;
    for (int u = 1; u <= V; u++) {
        ss << mapping[u] << (u < V ? " " : "");
    }
    cout << ss.str() << endl;
}

graph READ() {
    int V;
    cin >> V;
    graph G(V);
    for (int i = 0; i < 2 * V; i++) {
        int u, v;
        cin >> u >> v;
        G.add(u, v);
    }
    return G;
}

void solve() {
    int V;
    cin >> V >> V;

    const auto& original = graphs.at(V);
    WRITE_GRAPH(original);
    graph modified = READ();
    modified.hash_graph();
    vector<int> mapping(V + 1);

    for (int i = 1; i <= V; i++) {
        size_t hash = original.get_hash(i);
        int j = modified.get_vertex(hash);
        mapping[i] = j;
    }

    WRITE_MAPPING(mapping);
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    setup();
    unsigned T;
    cin >> T;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
