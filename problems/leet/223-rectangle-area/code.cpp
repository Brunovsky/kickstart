#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int computeArea(int A, int B, int C, int D, int E, int F, int G, int H) {
    int a1 = (C - A) * (D - B);
    int a2 = (G - E) * (H - F);
    // no intersection
    if (A >= G || C <= E || B >= H || D <= F)
      return a1 + a2;
    int x = max(A, E) - min(C, G);
    int y = max(B, F) - min(D, H);
    return a1 + (a2 - x * y); // prevent overflow
  }
};

// *****

int main() {
  return 0;
}
