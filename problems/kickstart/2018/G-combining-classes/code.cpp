#include <bits/stdc++.h>

using namespace std;
using u64 = uint64_t;

// *****

struct node_t {
  u64 hi = 0, lo = 0;
  u64 classes = 0;
  u64 prefix = 0;
};

u64 N, Q;
int O, P;
vector<u64> Hi, Lo, K;
vector<node_t> tree;
vector<u64> endp;

void tree_lr(int i) {
  if (i < O) {
    tree_lr(i << 1);
    tree_lr(i << 1 | 1);
    tree[i].hi = tree[i << 1].hi;
    tree[i].lo = tree[i << 1 | 1].lo;
  }
}

void add_class(u64 hi, u64 lo, int i) {
  // overlap
  if (hi > tree[i].lo && tree[i].hi > lo) {
    // contain
    if (hi >= tree[i].hi && tree[i].lo >= lo) {
      ++tree[i].classes;
    } else {
      assert(i < O);
      add_class(hi, lo, i << 1);
      add_class(hi, lo, i << 1 | 1);
    }
  }
}

void pushdown_classes(int i) {
  tree[i].classes += tree[i >> 1].classes;
  if (i < O) {
    pushdown_classes(i << 1);
    pushdown_classes(i << 1 | 1);
  }
}

bool node_cmp(const node_t &node, u64 k) {
  return node.prefix < k;
}

auto solve() {
  endp.clear();
  for (u64 l : Hi)
    endp.push_back(l);
  for (u64 r : Lo)
    endp.push_back(r);
  sort(endp.begin(), endp.end(), greater<u64>{});
  endp.erase(unique(endp.begin(), endp.end()), endp.end());
  O = endp.size() - 1;

  tree.assign(2 * O, {});
  P = 1;
  while (P < O)
    P <<= 1;

  auto obegin = tree.begin() + O;
  auto oend = tree.begin() + (2 * O);

  for (int i = O, j = 0; i < 2 * O; ++i, ++j) {
    tree[i].hi = endp[j];
    tree[i].lo = endp[j + 1];
  }

  rotate(obegin, obegin + (2 * O - P), oend); // rotate right P - O
  tree_lr(1);
  for (u64 n = 0; n < N; ++n)
    add_class(Hi[n], Lo[n], 1);
  pushdown_classes(1);
  rotate(obegin, obegin + (P - O), oend); // rotate left P - O

  for (int i = O; i < 2 * O; ++i) {
    u64 hi = tree[i].hi, lo = tree[i].lo, c = tree[i].classes;
    tree[i].prefix = (hi - lo) * c + tree[i - 1].prefix;
  }

  u64 sum = 0;

  for (u64 q = 1; q <= Q; ++q) {
    u64 k = K[q - 1];
    int i = lower_bound(obegin, oend, k, node_cmp) - tree.begin();
    if (i == 2 * O)
      continue;

    u64 a = tree[i - 1].prefix, c = tree[i].classes, hi = tree[i].hi;
    u64 S = hi - (k - a - 1) / c;
    sum += S * q;
  }

  return sum;
}

// *****

void reparse_test() {
  cin >> N >> Q >> ws;
  Hi.assign(N + 1, 0);
  Lo.assign(N + 1, 0);
  K.assign(Q + 3, 0);
  u64 x1, x2, x3, a1, b1, c1, m1;
  u64 y1, y2, y3, a2, b2, c2, m2;
  u64 z1, z2, z3, a3, b3, c3, m3;
  cin >> x1 >> x2 >> a1 >> b1 >> c1 >> m1 >> ws;
  cin >> y1 >> y2 >> a2 >> b2 >> c2 >> m2 >> ws;
  cin >> z1 >> z2 >> a3 >> b3 >> c3 >> m3 >> ws;

  Hi[0] = max(x1, y1) + 1;
  Lo[0] = min(x1, y1);
  Hi[1] = max(x2, y2) + 1;
  Lo[1] = min(x2, y2);

  for (u64 i = 2; i < N; ++i) {
    x3 = (a1 * x2 + b1 * x1 + c1) % m1;
    y3 = (a2 * y2 + b2 * y1 + c2) % m2;
    x1 = x2;
    x2 = x3;
    y1 = y2;
    y2 = y3;
    Hi[i] = max(x3, y3) + 1;
    Lo[i] = min(x3, y3);
  }

  K[0] = z1 + 1;
  K[1] = z2 + 1;

  for (u64 i = 2; i < Q; ++i) {
    z3 = (a3 * z2 + b3 * z1 + c3) % m3;
    z1 = z2;
    z2 = z3;
    K[i] = z3 + 1;
  }
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
