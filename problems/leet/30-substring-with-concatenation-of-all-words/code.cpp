#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  // words vector may contain repeated strings....
  vector<int> findSubstring(const string &s, const vector<string> &words) {
    if (words.empty())
      return {};
    int W = words.size(), S = s.length(), L = words[0].length();

    unordered_map<string, int> counts, totals;
    for (const string &word : words)
      counts[word] = 0, ++totals[word];
    int repeated = 0;

    vector<int> answers;

    for (int offset = 0; offset < L; ++offset) {
      int w = 1, i = offset, j = offset + L;
      while (j <= S) {
        string sub = s.substr(j - L, L);
        if (!counts.count(sub)) {
          for (auto &[word, count] : counts)
            count = 0;
          i = j, w = repeated = 0;
        } else {
          if (++counts.at(sub) > totals.at(sub))
            ++repeated;
          if (w == W) {
            if (!repeated)
              answers.push_back(i);
            string old = s.substr(i, L);
            if (--counts.at(old) >= totals.at(old))
              --repeated;
            i += L, --w;
          }
        }

        j += L, ++w;
        assert(w * L == j - i);
      }

      for (auto &[word, count] : counts)
        count = 0;
    }

    return answers;
  }
};

// *****

string test_str(const vector<int> &nums) {
  if (nums.empty())
    return "[]"s;
  string s = "["s + to_string(nums[0]);
  for (size_t i = 1; i < nums.size(); ++i)
    s += ","s + to_string(nums[i]);
  return s + "]"s;
}

int main() {
  vector<string> strings = {
      "wordgoodgoodgoodbestword",
      "barfoothefoobarman",
      "lingmindraboofooowingdingbarrwingmonkeypoundcake",
      "aaaaaaaa",
  };
  vector<vector<string>> words = {
      {"word", "good", "best", "good"},
      {"foo", "bar"},
      {"fooo", "barr", "wing", "ding", "wing"},
      {"aa", "aa", "aa"},
  };
  vector<vector<int>> expected = {{8}, {0, 9}, {13}, {0, 1, 2}};

  for (size_t i = 0; i < strings.size(); ++i) {
    auto actual = Solution{}.findSubstring(strings[i], words[i]);
    sort(actual.begin(), actual.end());
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("> Expected: %s\n", test_str(expected[i]).c_str());
    printf("> Actual:   %s\n", test_str(actual).c_str());
  }

  return 0;
}
