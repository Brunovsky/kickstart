#ifndef SUFFIX_AUTOMATON_HPP
#define SUFFIX_AUTOMATON_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Suffix DFA string automation
 * Edit A and chash before using. Add extra preprocessing at the end of preprocess().
 * Requires 8(A+c)N bytes, where N is the length of the text. Not adequate if A>50.
 *
 * Complexity: O(AN)  construction (includes preprocess()), O(AN) space
 *
 * Usage is straightforward if the automaton does not need to grow. If it does, call
 * preprocess() after growing the automaton to use the methods that need preprocessing.
 * Usage:
 *     suffix_automaton sa(text);
 *     bool found = sa.contains(word);
 *     long n = sa.count_distinct_substrings();
 *     int m = sa.count_matches(word);
 *     ...
 */
template <typename Vec = string, typename T = typename Vec::value_type>
struct suffix_automaton {
    static constexpr int A = 26;
    static constexpr int chash(T value) { return value - 'a'; }

    struct Node {
        int next[A] = {};
        int len = 0, ch = 0, numpos = 0;
        int link = 0; // suffix link
        Node() = default;
        explicit Node(int len, int ch) : len(len), ch(ch) {}
    };

    vector<Node> node;
    int last = 1; // node[0] is empty; last is id of node with entire string
    bool is_preprocessed = false;

    suffix_automaton() = default;
    explicit suffix_automaton(const Vec& text) : node(2) {
        node.reserve(2 * text.size() + 2);
        for (auto c : text) {
            extend(c);
        }
        preprocess();
    }

    int extend(T value) {
        int c = chash(value);
        int v = node.size(), p = last;
        node.emplace_back(node[p].len + 1, c);
        while (p && !node[p].next[c]) {
            node[p].next[c] = v, p = node[p].link;
        }
        if (p == 0)
            node[v].link = 1;
        else {
            int q = node[p].next[c];
            if (node[p].len + 1 == node[q].len)
                node[v].link = q;
            else {
                int u = node.size();
                node.emplace_back(node[q]);
                node[u].len = node[p].len + 1, node[u].ch = c;
                while (p && node[p].next[c] == q) {
                    node[p].next[c] = u, p = node[p].link;
                }
                node[q].link = node[v].link = u;
            }
        }
        is_preprocessed = false;
        return last = v;
    }

    void preprocess() {
        vector<Node> tmp = node;
        vector<int> cnt(node[last].len + 1, 0), p(num_nodes());

        for (int v = 0; v < num_nodes(); v++)
            cnt[node[v].len]++;
        for (int len = 1; len <= node[last].len; len++)
            cnt[len] += cnt[len - 1];
        for (int v = num_nodes() - 1; v >= 0; v--) // fix 0 and 1
            p[v] = --cnt[node[v].len];

        for (int v = 0; v < num_nodes(); v++) {
            auto& new_node = tmp[p[v]];
            new_node.len = node[v].len;
            new_node.link = p[node[v].link];
            new_node.ch = node[v].ch;
            for (int c = 0; c < A; c++) {
                new_node.next[c] = p[node[v].next[c]];
            }
        }
        swap(node, tmp), is_preprocessed = true;

        // Preprocess: here node[1..num_nodes) is sorted topologically.
        // numpos: number of positions where state v can be found.
        for (int v = num_nodes() - 1; v >= 1; v--) {
            node[v].numpos++;
            node[node[v].link].numpos += node[v].numpos;
        }
        node[0].numpos = 0;
    }

    int num_nodes() const { return node.size(); }

    int get_state(const Vec& word) const {
        int v = 1;
        for (int i = 0, W = word.size(); i < W && v; i++) {
            v = node[v].next[chash(word[i])];
        }
        return v;
    }

    // O(AN) Count the number of distinct substrings (including the empty substring)
    long count_distinct_substrings() const {
        assert(is_preprocessed);
        vector<long> dp(num_nodes(), 1);
        dp[0] = 0;
        for (int v = num_nodes() - 1; v >= 1; v--) {
            for (int c = 0; c < A; c++) {
                dp[v] += dp[node[v].next[c]];
            }
        }
        return dp[1];
    }

    // O(W) Does this text contain word
    bool contains(const Vec& word) const { return get_state(word) != 0; }

    // O(W) Length of the longest prefix of word that matches a substring of this text
    int longest_prefix(const Vec& word) const {
        for (int v = 1, i = 0, W = word.size(); i < W; i++) {
            v = node[v].next[chash(word[i])];
            if (v == 0) {
                return i;
            }
        }
        return word.size();
    }

    // O(W) Number of times that word appears in this text
    int count_matches(const Vec& word) const { return node[get_state(word)].numpos; }
};

#endif // SUFFIX_AUTOMATON_HPP
