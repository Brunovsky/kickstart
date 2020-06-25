#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  void push(string &roman, int &num, int base, char one, char five,
            char ten) const {
    if (num >= 9 * base) {
      num -= 9 * base;
      roman += one;
      roman += ten;
    } else if (num >= 5 * base) {
      num -= 5 * base;
      roman += five;
    } else if (num >= 4 * base) {
      num -= 4 * base;
      roman += one;
      roman += five;
    }
    while (num >= base) {
      num -= base;
      roman += one;
    }
  }

  string intToRoman(int num) {
    string roman;
    push(roman, num, 1000, 'M', 'A', 'B'); // A, B unused
    push(roman, num, 100, 'C', 'D', 'M');
    push(roman, num, 10, 'X', 'L', 'C');
    push(roman, num, 1, 'I', 'V', 'X');
    return roman;
  }
};

// *****

int main() {
  return 0;
}
