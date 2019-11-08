#include <bits/stdc++.h>

using namespace std;

int main() {
  int T = RAND_MAX / 2;
  for (int r = 0; r < 100; ++r) {
    for (int c = 0; c < 100; ++c) {
      if (rand() < T) {
        cout << 'A';
      } else {
        cout << 'B';
      }
    }
    cout << endl;
  }
  return 0;
}
