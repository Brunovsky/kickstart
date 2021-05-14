#ifndef MAP_SUFFIX_AUTOMATON
#define MAP_SUFFIX_AUTOMATON

#include <bits/stdc++.h>

using namespace std;

template <typename Vec = string, typename T = typename Vec::value_type>
struct map_suffix_automaton {
    // We do not need alphabet size :)
    static constexpr int chash(T value) { return value; }

    struct Node {
        int len = 0, link = 0;
        int ch = 0;
        int numpos = 0;
        bool terminal = false;
        Node() = default;
        Node(int len, int ch) : len(len), ch(ch) {}
    };

    int V, E, last = 1; // node[0] is empty; last is id of node with entire string
    vector<Node> node;
    vector<map<int, int>> edge;
    vector<int> pi;

    map_suffix_automaton() : V(2), E(0), node(2), edge(2) {}
    explicit map_suffix_automaton(const Vec& text) : map_suffix_automaton() {
        extend(text);
        preprocess();
    }

    int get_link(int u, int c) const {
        auto it = edge[u].find(c);
        return it != edge[u].end() ? it->second : 0;
    }
    void set_link(int u, int c, int v) { edge[u].at(c) = v; }
    void add_link(int u, int c, int v) { edge[u].emplace(c, v), E++; }
    int add_node(int len, int ch) {
        return node.emplace_back(len, ch), edge.push_back({}), V++;
    }
    int clone_node(int u, int len, int ch) {
        int v = add_node(len, ch);
        edge[v] = edge[u], node[v].link = node[u].link;
        return v;
    }

    void extend(const Vec& s) {
        for (char c : s) {
            extend(c);
        }
    }

    void extend(const T& value) {
        int c = chash(value), p = last;
        int v = add_node(node[p].len + 1, c);
        while (p && !get_link(p, c)) {
            add_link(p, c, v), p = node[p].link;
        }
        if (p == 0)
            node[v].link = 1;
        else {
            int q = get_link(p, c);
            if (node[p].len + 1 == node[q].len)
                node[v].link = q;
            else {
                int u = clone_node(q, node[p].len + 1, c);
                while (p && get_link(p, c) == q) {
                    set_link(p, c, u), p = node[p].link;
                }
                node[q].link = node[v].link = u;
                node[q].terminal = false;
            }
        }
        last = v;
    }

    void preprocess() {
        vector<int> cnt(node[last].len + 1), pos(V);
        pi.resize(V);

        for (int v = 0; v < V; v++)
            cnt[node[v].len]++;
        for (int len = 1; len <= node[last].len; len++)
            cnt[len] += cnt[len - 1];
        for (int v = V - 1; v >= 0; v--)
            pos[v] = --cnt[node[v].len];
        for (int v = 0; v < V; v++)
            pi[pos[v]] = v;

        // topological order: pi[0], pi[1], pi[2], ...
        // numpos: number of positions where state v can be found.
        for (int i = V - 1, v = pi[i]; i >= 1; i--, v = pi[i]) {
            node[v].numpos++;
            node[node[v].link].numpos += node[v].numpos;
        }
        node[0].numpos = 0;

        // terminal: whether a state is terminal (corresponds to a suffix)
        int u = last;
        do {
            node[u].terminal = true, u = node[u].link;
        } while (u > 1);
    }

    int get_state(const Vec& word) const {
        int v = 1;
        for (int i = 0, W = word.size(); i < W && v; i++) {
            v = get_link(v, chash(word[i]));
        }
        return v;
    }

    // O(N) Count the number of distinct substrings (including the empty substring)
    long count_distinct_substrings() const {
        vector<long> dp(V, 1);
        dp[0] = 0;
        for (int i = V - 1, v = pi[i]; i >= 1; i--, v = pi[i]) {
            for (auto [c, u] : edge[v]) {
                dp[v] += dp[u];
            }
        }
        return dp[1];
    }

    // O(W) Does this text contain word
    bool contains(const Vec& word) const { return get_state(word) != 0; }

    // O(W) Length of the longest prefix of word that matches a substring of this text
    int longest_prefix(const Vec& word) const {
        for (int v = 1, i = 0, W = word.size(); i < W; i++) {
            v = get_link(v, chash(word[i]));
            if (v == 0) {
                return i;
            }
        }
        return word.size();
    }

    // O(W) Number of times that word appears in this text
    int count_matches(const Vec& word) const { return node[get_state(word)].numpos; }
};

#endif // MAP_SUFFIX_AUTOMATON
