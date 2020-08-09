#include <bits/stdc++.h>

using namespace std;

// *****

string ARRAY(const vector<vector<int>> &b) {
  string s;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++)
      s += '0' + b[i][j];
  return s;
}

constexpr int swaps[][2] = {{0, 3}, {1, 4}, {2, 5}, {0, 1},
                            {1, 2}, {3, 4}, {4, 5}};

class Solution {
  static unordered_map<string, int> moves;

  static void init() {
    string init = "123450";
    vector<string> frontier;
    vector<string> next_frontier;
    moves[init] = 0;
    int dist = 1;
    frontier.push_back(init);

    while (!frontier.empty()) {
      for (const string &state : frontier) {
        for (auto dd : swaps) {
          int L = dd[0], R = dd[1];
          if (state[L] == '0' || state[R] == '0') {
            auto adj = state;
            swap(adj[L], adj[R]);
            if (!moves.count(adj)) {
              moves[adj] = dist;
              next_frontier.push_back(adj);
            }
          }
        }
      }
      swap(frontier, next_frontier);
      next_frontier.clear();
      dist++;
    }
    printf("size: %lu | dist: %d\n", moves.size(), dist);
  }

public:
  Solution() {
    if (moves.empty())
      init();
  }

  int slidingPuzzle(const vector<vector<int>> &board) {
    auto arr = ARRAY(board);
    return moves.count(arr) ? moves.at(arr) : -1;
  }
};

unordered_map<string, int> Solution::moves;

// *****

int main() {
  Solution solution;
  cout << solution.slidingPuzzle({{1, 2, 3}, {4, 0, 5}}) << endl;
  cout << solution.slidingPuzzle({{1, 2, 3}, {5, 4, 0}}) << endl;
  cout << solution.slidingPuzzle({{4, 1, 2}, {5, 0, 3}}) << endl;
  cout << solution.slidingPuzzle({{3, 2, 4}, {1, 5, 0}}) << endl;
  return 0;
}
