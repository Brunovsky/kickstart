#include <bits/stdc++.h>

using namespace std;

// *****

vector<string> cache = {"", "1", "11", "21", "1211", "111221"};

void compute() {
  const string &str = cache.back();
  string say;
  size_t i = 0, S = str.size();
  while (i < S) {
    size_t j = i + 1;
    while (j < S && str[i] == str[j])
      ++j;
    say += (j - i) + '0';
    say += str[i];
    i = j;
  }
  cache.push_back(move(say));
}

class Solution {
public:
  string countAndSay(int n) {
    if (n >= int(cache.size()))
      for (int i = cache.size(); i <= n; ++i)
        compute();
    return cache[n];
  }
};

// *****

int main() {
  return 0;
}
