#include "../struct/y_combinator.hpp"

#define AUTO [[maybe_unused]] auto

/**
 * A repertoir of techniques on a collection of words
 */
void word_collection_sample(vector<string> words) {
    int N = words.size();
    constexpr int A = 26;
    constexpr auto chash = [](char c) { return c - 'a'; };

    // Compute LCP of two words starting at s
    auto lcp_two = [&](const string& a, const string& b, int s = 0) {
        int S = min(a.size(), b.size());
        for (int i = s; i < S; i++) {
            if (a[i] != b[i]) {
                return i;
            }
        }
        return S;
    };

    // ***** Compute representative word from collection of characters
    {
        vector<string> chars_rep(N);
        unordered_map<string, vector<string>> by_chars_rep;
        for (int i = 0; i < N; i++) {
            string s = words[i];
            sort(begin(s), end(s));
            chars_rep[i] = s;
            by_chars_rep[s].push_back(words[i]);
        }
    }

    // ***** Sort the words and compute LCPs
    {
        vector<string> sorted = words;
        sort(begin(sorted), end(sorted));

        vector<int> sorted_lcp(N - 1);
        for (int i = 0; i < N - 1; i++) {
            sorted_lcp[i] = lcp_two(sorted[i], sorted[i + 1]);
        }
    }

    // ***** Build a standard uncompressed trie. Assume no two words are the same.
    {
        struct TrieNode {
            array<int, A> next = {};
            int parent = 0;
            int id = -1;
            int depth = 0;
            int overlap = 0;
            explicit TrieNode(int parent, int depth) : parent(parent), depth(depth) {}
        };
        vector<TrieNode> trie(1, TrieNode(0, 0));
        vector<int> where_trie(N);

        for (int i = 0; i < N; i++) {
            int u = 0, W = words[i].size();
            while (trie[u].depth < W) {
                int c = chash(words[i][trie[u].depth]);
                if (!trie[u].next[c]) {
                    trie[u].next[c] = trie.size();
                    trie.emplace_back(u, trie[u].depth + 1);
                }
                u = trie[u].next[c];
                trie[u].overlap++;
            }
            trie[u].id = i;
            where_trie[i] = u;
        }

        int T = trie.size();
        vector<vector<int>> trie_children(T);

        for (int u = 1; u < T; u++) {
            trie_children[trie[u].parent].push_back(u);
        }
    }

    // ***** Build a compressed trie. Assume no two words are the same
    {
        struct CTrieNode {
            array<int, A> next = {};
            int parent = 0;
            int id = -1;
            int depth = 0;
            int overlap = 0;
            int len = 0;
            int sample = 0;
            CTrieNode(int parent, int depth, int len, int sample)
                : parent(parent), depth(depth), len(len), sample(sample) {}
        };

        vector<CTrieNode> ctrie(1, CTrieNode(0, 0, 0, 0));
        vector<int> where_ctrie(N);

        for (int i = 0; i < N; i++) {
            int u = 0, W = words[i].size();
            while (ctrie[u].depth < W) {
                int j = ctrie[u].depth;
                int c = chash(words[i][j]);
                if (!ctrie[u].next[c]) {
                    ctrie[u].next[c] = ctrie.size();
                    ctrie.emplace_back(u, W, W - j, i);
                    u = ctrie[u].next[c];
                } else {
                    int b = 1;
                    int v = ctrie[u].next[c];
                    int o = ctrie[v].sample;
                    int M = min(ctrie[v].depth, W);
                    while (j + b < M && words[i][j + b] == words[o][j + b]) {
                        b++;
                    }
                    if (j + b == ctrie[v].depth) {
                        u = v;
                    } else {
                        int c0 = chash(words[o][j + b]);
                        int w = ctrie.size();
                        ctrie.emplace_back(u, j + b, b, i);
                        ctrie[u].next[c] = w;
                        ctrie[w].next[c0] = v;
                        ctrie[v].len -= b;
                        ctrie[v].parent = w;
                        u = w;
                    }
                }
                ctrie[u].overlap++;
            }
            ctrie[u].id = i;
            where_ctrie[i] = u;
        }

        int T = ctrie.size();
        vector<vector<int>> ctrie_children(T);

        for (int u = 1; u < T; u++) {
            ctrie_children[ctrie[u].parent].push_back(u);
        }
    }
}
