#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;

// *****

using i64 = int64_t;
using happy = pair<i64, i64>;

using max_tree = tree<happy, null_type, greater<happy>, rb_tree_tag,
                      tree_order_statistics_node_update>;

int N, M;
i64 H;
i64 A[20];
i64 B[20];

inline i64 pow3(int n) {
  i64 power = 1;
  while (n--) {
    power *= 3;
  }
  return power;
}

auto populate(int M1, int M2) {
  i64 a = 0;
  i64 b = 0;
  for (int i = M1; i < M2; i++) {
    a += A[i];
  }

  vector<happy> sums;

  // bit is 0 -> A does shift
  // bit is 1 -> B does shift
  // bit is 2 -> A and B do shift
  int flip_mask = 0;
  int max_flip_mask = pow3(M2 - M1);
  int state = 0;

  while (flip_mask++ < max_flip_mask) {
    sums.push_back({a, b});

    int i = M1, bit = flip_mask, flip = 1;
    while ((bit % 3) == 0) {
      ++i, bit /= 3, flip *= 3;
    }

    switch ((state / flip) % 3) {
    case 0: // A -> B
      a -= A[i];
      b += B[i];
      state += flip;
      break;
    case 1: // B -> A and B
      a += A[i];
      state += flip;
      break;
    case 2: // A and B -> A
      b -= B[i];
      state -= 2 * flip;
      break;
    }
  }

  return sums;
}

auto solve() {
  i64 a = 0;
  i64 b = 0;

  cin >> N >> H >> ws;
  for (int i = 0; i < N; i++) {
    cin >> A[i] >> ws;
  }
  for (int i = 0; i < N; i++) {
    cin >> B[i] >> ws;
  }

  M = N / 2;
  auto L1 = populate(0, M);
  auto L2 = populate(M, N);
  sort(L1.begin(), L1.end(), less<happy>{});    // increasing
  sort(L2.begin(), L2.end(), greater<happy>{}); // decreasing
  int S1 = L1.size();
  int S2 = L2.size();

  size_t count = 0;
  i64 max_a = -1;
  int i = 0, j = 0;
  max_tree tree;

  while (i < S1) {
    i64 a1 = L1[i].first, b1 = L1[i].second;
    if (a1 > max_a) {
      while (j < S2 && L2[j].first + a1 >= H) {
        i64 b2 = L2[j].second;
        tree.insert({b2, j});
        j++;
      }
      max_a = a1;
    }
    count += tree.order_of_key({H - b1, -1});
    i++;
  }

  return count;
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
