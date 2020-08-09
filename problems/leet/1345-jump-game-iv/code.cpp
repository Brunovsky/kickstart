#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int minJumps(const vector<int> &arr) {
    int n = arr.size();
    unordered_map<int, vector<int>> idx;
    for (int i = 0; i < n; i++) {
      idx[arr[i]].push_back(i);
    }
    vector<int> frontier;
    vector<int> next_frontier;
    vector<bool> vis_idx(n, false);
    unordered_map<int, bool> vis_val;
    vis_idx[0] = true;
    frontier.push_back(0);
    int jumps = 0;

    while (!vis_idx[n - 1]) {
      jumps++;
      for (int i : frontier) {
        // walk left
        if (i > 0 && !vis_idx[i - 1]) {
          vis_idx[i - 1] = true;
          next_frontier.push_back(i - 1);
        }
        // walk right
        if (i < n - 1 && !vis_idx[i + 1]) {
          vis_idx[i + 1] = true;
          next_frontier.push_back(i + 1);
        }
        // jump
        if (!vis_val[arr[i]]) {
          vis_val[arr[i]] = true;
          for (int j : idx[arr[i]]) {
            if (!vis_idx[j]) {
              vis_idx[j] = true;
              next_frontier.push_back(j);
            }
          }
        }
      }
      frontier = move(next_frontier);
    }
    return jumps;
  }
};

// *****

int main() {
  cout << Solution{}.minJumps({100, -23, -23, 404, 100, 23, 23, 23, 3, 404}) << endl;
  cout << Solution{}.minJumps({7}) << endl;
  cout << Solution{}.minJumps({7, 6, 9, 6, 9, 6, 9, 7}) << endl;
  cout << Solution{}.minJumps({6, 1, 9}) << endl;
  cout << Solution{}.minJumps({11, 22, 7, 7, 7, 7, 7, 7, 7, 22, 13}) << endl;
  return 0;
}
