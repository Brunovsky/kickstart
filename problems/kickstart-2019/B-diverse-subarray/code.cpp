#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int MAX_A = 100'000;

int N, P, S;
vector<int> A;
vector<int> sum;
vector<int> pre;

vector<int> finger_1;
vector<int> finger_S;

int tree_index(int i) {
  const int bot = 2 * N - P;
  int treei = (i >= bot) ? (i - N + P) : (i + P);
  return treei;
}

void populate() {
  P = ((N & (N - 1)) == 0) ? N : N << 1;
  while ((P & (P - 1)) != 0) P &= P - 1;

  finger_1.assign(N + 1, N);
  finger_S.assign(N + 1, N);
  sum.assign(2 * N, 0);
  pre.assign(2 * N, 0);

  int count[MAX_A + 1] {};
  list<int> previous[MAX_A + 1] {};

  for (int i = 0; i < N; ++i) {
    int c = ++count[A[i]];

    if (c > 1) {
      int j = previous[A[i]].back();
      finger_1[j] = i;
    }

    previous[A[i]].push_back(i);

    if (c > S) {
      int j = previous[A[i]].front();
      previous[A[i]].pop_front();
      finger_S[j] = i;
    }

    int treei = tree_index(i);

    if (c <= S) {
      pre[treei] = sum[treei] = 1;
    } else if (c == S + 1) {
      pre[treei] = sum[treei] = -S;
    } else /* c > S + 1 */ {
      pre[treei] = sum[treei] = 0;
    }
  }
}

void dive(int i) {
  if (i < N) {
    dive(i << 1);
    dive(i << 1 | 1);
    sum[i] = sum[i << 1] + sum[i << 1 | 1];
    pre[i] = max(pre[i << 1], sum[i << 1] + pre[i << 1 | 1]);
  }
}

void pushup(int i) {
  if (i >= N) return;
  sum[i] = sum[i << 1] + sum[i << 1 | 1];
  pre[i] = max(pre[i << 1], sum[i << 1] + pre[i << 1 | 1]);
  if (i > 1) pushup(i >> 1);
}

auto solve() {
  populate();
  dive(1);

  int max = pre[1];

  for (int i = 0; i < N; ++i) {
    int j = finger_S[i];
    int k = finger_1[j];

    int ti = tree_index(i);
    pre[ti] = sum[ti] = 0;
    pushup(ti >> 1);

    if (j < N) {
      int tj = tree_index(j);
      pre[tj] = sum[tj] = 1;
      pushup(tj >> 1);
    }

    if (k < N) {
      int tk = tree_index(k);
      pre[tk] = sum[tk] = -S;
      pushup(tk >> 1);
    }

    if (pre[1] > max) max = pre[1];
  }

  return max;
}

// *****

void reparse_test() {
  cin >> N >> S >> ws;
  A.assign(N, 0);
  for (int i = 0; i < N; ++i) cin >> A[i] >> ws;
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
