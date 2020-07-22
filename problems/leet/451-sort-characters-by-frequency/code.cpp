#include <bits/stdc++.h>

using namespace std;

// *****

struct data_t {
  char c;
  uint count;
};

inline bool cmp_count(const data_t &lhs, const data_t &rhs) {
  return lhs.count > rhs.count;
}

class Solution {
public:
  string frequencySort(string s) {
    if (s.empty())
      return s;

    vector<data_t> data(256);

    for (int i = 0; i < 256; ++i)
      data[i] = {char(i), 0};

    for (size_t i = 0; i < s.size(); ++i)
      data[static_cast<unsigned char>(s[i])].count++;

    sort(data.begin(), data.end(), cmp_count);

    size_t i = 0, j = 0;
    while (true) {
      while (j < 256 && data[j].count == 0)
        ++j;
      if (j == 256)
        break;

      while (data[j].count-- > 0)
        s[i++] = data[j].c;
      ++j;
    }

    return s;
  }
};

// *****

int main() {
  return 0;
}
