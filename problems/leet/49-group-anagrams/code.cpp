#include <bits/stdc++.h>

using namespace std;

// *****

// Nothing spectacular about this but it beats 98%...?

class Solution {
public:
  vector<vector<string>> groupAnagrams(vector<string> &strs) {
    unordered_map<string, vector<string>> groups;
    for (string str : strs) {
      string key = str;
      sort(key.begin(), key.end());
      groups[key].push_back(move(str));
    }
    vector<vector<string>> answer;
    answer.reserve(groups.size());
    for (auto &[key, list] : groups)
      answer.push_back(move(list));
    return answer;
  }
};

// *****

int main() {
  return 0;
}
