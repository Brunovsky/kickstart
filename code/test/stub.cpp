#include "test_utils.hpp"
#include "../code/hacking.hpp"
#include "../code/stub/rooted_tree.cpp"
#include "../code/stub/word_collection.cpp"

#define long int64_t

void run_word_collection_sample() {
    const int N = 1000;
    vector<string> words(N);
    for (int i = 0; i < N; i++) {
        int W = randint(100, 500);
        words[i] = generate_any_string(W, 'a', 'f');
    }
    word_collection_sample(words);
}

void run_rooted_tree_sample() {
    const int N = 20;
    vector<int> parent = parent_sample(N, 1);
    vector<vector<int>> adj(N + 1);
    for (int i = 2; i <= N; i++) {
        adj[parent[i]].push_back(i);
        adj[i].push_back(parent[i]);
    }
    vector<int> arr = int_gen<int>(N + 1, 0, 100);
    rooted_tree_sample(adj, 1, arr);
}

int main() {
    run_word_collection_sample();
    run_rooted_tree_sample();
    return 0;
}
