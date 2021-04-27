#ifndef AHO_CORASICK_HPP
#define AHO_CORASICK_HPP

#include "../hash.hpp"

/**
 * Aho-Corasick string automaton
 * Edit A and chash before using (hopefully they don't need to be dynamic)
 * Requires 4(A+c)S bytes where S is the number of states.
 *
 * Complexity: O(AS+M) construction, where S=#states and M=#dict length.
 *             O(AS) space
 *             O(N) for main queries
 * Reference: https://github.com/indy256/codelibrary
 */
template <typename Vec = string, typename T = typename Vec::value_type>
struct aho_corasick {
    static constexpr int A = 26;
    static constexpr int chash(T v) { return v - 'a'; }

    struct Node {
        int next[A] = {}, parent = 0, depth = 0, ch = 0;
        int escape = 0, link = 0; // nearest leaf / suffix link
        int nmatches = 0, wordid = -1;
        Node() = default;
        Node(int parent, int depth, int ch) : parent(parent), depth(depth), ch(ch) {}
    };

    vector<Node> node;

    aho_corasick() = default;
    explicit aho_corasick(const vector<Vec>& words) : node(1) {
        for (int i = 0, W = words.size(); i < W; i++) {
            assert(!words[i].empty());
            int v = 0, depth = 1;
            for (const auto& ch : words[i]) {
                if (!node[v].next[chash(ch)]) {
                    node[v].next[chash(ch)] = node.size();
                    node.emplace_back(v, depth, chash(ch));
                }
                v = node[v].next[chash(ch)], depth++;
            }
            node[v].escape = v;
            // ignore repeated words. add other properties: word mask, sum of lengths...
            node[v].nmatches = 1;
            node[v].wordid = i;
        }

        vector<int> bfs(num_states());
        int s = 0, t = 1;
        while (s < t) {
            int v = bfs[s++], u = node[v].link;
            if (node[v].escape == 0) {
                node[v].escape = node[u].escape;
            }
            for (int c = 0; c < A; c++) {
                int& w = node[v].next[c];
                if (w) {
                    bfs[t++] = w;
                    node[w].link = v ? node[u].next[c] : 0;
                } else {
                    w = node[u].next[c];
                }
            }
            // dp extra properties
            node[v].nmatches += node[u].nmatches;
        }
        assert(t == num_states());
    }

    int num_states() const { return node.size(); }

    int count_unique_matches(const Vec& text) const {
        int matches = 0;
        for (int v = 0, i = 0, N = text.size(); i < N; i++) {
            v = node[v].next[chash(text[i])];
            matches += node[v].escape > 0;
        }
        return matches;
    }

    long count_matches(const Vec& text) const {
        long matches = 0;
        for (int v = 0, i = 0, N = text.size(); i < N; i++) {
            v = node[v].next[chash(text[i])];
            matches += node[v].nmatches;
        }
        return matches;
    }

    // For each index i, find the longest dictionary word ending at text[i] (inclusive)
    vector<int> longest_each_index(const Vec& text) const {
        vector<int> longest(text.size(), -1);
        for (int v = 0, i = 0, N = text.size(); i < N; i++) {
            v = node[v].next[chash(text[i])];
            longest[i] = node[node[v].escape].wordid;
        }
        return longest;
    }

    // Call fn(i, wordid) for every match <i, longest wordid>
    template <typename Fn>
    void visit_longest_each_index(const Vec& text, Fn&& fn, bool skipbad = true) const {
        for (int v = 0, i = 0, N = text.size(); i < N; i++) {
            v = node[v].next[chash(text[i])];
            int w = node[node[v].escape].wordid;
            if (w != -1 || !skipbad)
                fn(i, w);
        }
    }

    // Call fn(i, wordid) for every match <i, wordid>
    template <typename Fn>
    void visit_all(const Vec& text, Fn&& fn) const {
        for (int v = 0, i = 0, N = text.size(); i < N; i++) {
            v = node[v].next[chash(text[i])];
            int u = node[v].escape;
            while (u != 0)
                fn(i, node[u].wordid), u = node[node[u].link].escape;
        }
    }

    // Find first occurrence of any dictionary word in text. Returns <i, longest wordid>
    pair<int, int> find_first(const Vec& text) const {
        for (int v = 0, i = 0, N = text.size(); i < N; i++) {
            v = node[v].next[chash(text[i])];
            int w = node[node[v].escape].wordid;
            if (w != 0)
                return {i, w};
        }
        return {-1, -1};
    }
};

#endif // AHO_CORASICK_HPP
