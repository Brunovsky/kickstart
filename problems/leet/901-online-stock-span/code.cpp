#include <bits/stdc++.h>

using namespace std;

// *****

struct stock {
  int price;
  int index;
};

class StockSpanner {
  int N = 0;
  stack<stock> finger;

public:
  StockSpanner() {}

  int next(int price) {
    while (!finger.empty() && finger.top().price <= price) {
      finger.pop();
    }
    int dist = finger.empty() ? N + 1 : N - finger.top().index;
    finger.push({price, N});
    N++;
    return dist;
  }
};

// *****

int main() {
  return 0;
}
