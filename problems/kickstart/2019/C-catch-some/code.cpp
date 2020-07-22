#include <bits/stdc++.h>

using namespace std;

// *****

int N, K;
vector<int> P, A;
vector<vector<int>> B;
set<int> S;

constexpr int INFINT = INT32_MAX - 200'500;

struct Node {
  int H = INFINT, T = INFINT;
};

auto solve() {
  cin >> N >> K >> ws;
  S.clear();
  P.assign(N, 0);
  A.assign(N, 0);
  for (int i = 0; i < N; ++i)
    cin >> P[i];
  for (int i = 0; i < N; ++i)
    cin >> A[i];

  B.assign(100'001, {});
  for (int i = 0; i < N; ++i)
    B[A[i]].push_back(P[i]);
  for (int i = 0; i < N; ++i)
    S.insert(A[i]);

  const int C = S.size();

  using matrix_t = vector<vector<Node>>;
  matrix_t dyn(C + 1, {size_t(K + 1), {INFINT, INFINT}});
  // dyn[c][k].?:
  // min dist for colors 1..c, to find k dogs. H for head, T for tail.

  dyn[0][0] = {0, 0};

  vector<int> partial;

  int a = 1, seen = 0;
  for (int c : S) {
    const int dogs = B[c].size();
    seen += dogs;
    dyn[a][0] = {0, 0};
    sort(B[c].begin(), B[c].end()); // this is necessary, really Google?

    partial.push_back(seen);

    for (int k = 1, maxk = min(K, seen); k <= maxk; ++k) {
      dyn[a][k].H = dyn[a - 1][k].H;
      dyn[a][k].T = dyn[a - 1][k].T;

      for (int l = 1, maxl = min(dogs, k); l <= maxl; ++l) {
        dyn[a][k].H = min(dyn[a][k].H, //
                          dyn[a - 1][k - l].H + 2 * B[c][l - 1]);
        dyn[a][k].T = min(dyn[a][k].T,                               //
                          min(dyn[a - 1][k - l].T + 2 * B[c][l - 1], //
                              dyn[a - 1][k - l].H + 1 * B[c][l - 1]));
      }
    }

    ++a;
  }

  return dyn[C][K].T;
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
