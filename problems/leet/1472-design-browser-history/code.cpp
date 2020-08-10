#include <bits/stdc++.h>

using namespace std;

// *****

class BrowserHistory {
  vector<string> history;
  int n = 0;

public:
  BrowserHistory(string homepage) {
    history.push_back(move(homepage));
    history.reserve(200);
  }

  void visit(string url) {
    history.resize(n + 2);
    history[++n] = move(url);
  }

  string back(int steps) {
    n = max(0, n - steps);
    return history[n];
  }

  string forward(int steps) {
    int h = history.size();
    n = min(n + steps, h - 1);
    return history[n];
  }
};

// *****

int main() {
  return 0;
}
