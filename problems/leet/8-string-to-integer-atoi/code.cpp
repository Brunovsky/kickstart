#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int myAtoi(const string &str) {
    int64_t num = 0;
    int sign = 1;
    size_t i = 0, S = str.size();
    while (i < S && str[i] == ' ')
      i++;

    if (str[i] == '+')
      i++;
    else if (str[i] == '-')
      sign = -1, i++;

    while (i < S && str[i] >= '0' && str[i] <= '9') {
      num = num * 10 + sign * int64_t(str[i++] - '0');
      if (num < INT_MIN)
        return INT_MIN;
      if (num > INT_MAX)
        return INT_MAX;
    }
    return num;
  }
};

// *****

int main() {
  const vector<string> tests = {"42", "-137", "170123", "-999999"};
  const vector<int> expected = {42, -137, 170123, -999999};

  for (size_t i = 0; i < tests.size(); ++i) {
    int actual = Solution{}.myAtoi(tests[i]);
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("> Expected: %d\n", expected[i]);
    printf("> Actual:   %d\n", actual);
  }

  return 0;
}
