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

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
