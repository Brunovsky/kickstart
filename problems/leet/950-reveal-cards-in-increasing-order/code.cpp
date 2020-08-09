#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> deckRevealedIncreasing(vector<int> &deck) {
    int N = deck.size();
    vector<int> ans(N);
    sort(begin(deck), end(deck));
    deque<int> idx(N);
    iota(begin(idx), end(idx), 0);
    for (int z = 0; z + 1 < N; z++) {
      ans[idx[0]] = deck[z];
      idx.pop_front();
      idx.push_back(idx[0]);
      idx.pop_front();
    }
    ans[idx[0]] = deck[N - 1];
    return ans;
  }
};

// *****

int main() {
  int N = 20;
  vector<int> deck = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                      11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  auto res = Solution{}.deckRevealedIncreasing(deck);
  deque<int> dq(begin(res), end(res));
  for (int i = 0; i + 1 < N; i++) {
    cout << dq[0] << ' ';
    dq.pop_front();
    dq.push_back(dq.front());
    dq.pop_front();
  }
  cout << dq.front();
  return 0;
}
