#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string multiply(string num1, string num2) {
    if (num1 == "0" || num2 == "0")
      return "0";

    int N1 = num1.size(), N2 = num2.size();
    vector<int> digits(N1 + N2, 0);
    for (int i = 0; i < N1; ++i) {
      int n1 = num1[i] - '0';
      for (int j = 0; j < N2; ++j) {
        int n2 = num2[j] - '0';
        int mul = n1 * n2;
        digits[i + j] += mul / 10;
        digits[i + j + 1] += mul % 10;
      }
    }
    int carry = 0;
    for (int k = N1 + N2 - 1; k >= 0; --k) {
      int sum = digits[k] + carry;
      carry = sum / 10;
      digits[k] = sum % 10;
    }

    string result;
    result.reserve(N1 + N2);
    int i = !digits[0]; // skip leading zero
    while (i < N1 + N2) {
      result += digits[i] + '0';
      i++;
    }
    return result;
  }
};

// *****

int main() {
  return 0;
}
