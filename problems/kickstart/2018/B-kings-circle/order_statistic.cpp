#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
namespace gnu = __gnu_pbds;

using Cafe = pair<int, int>;

using min_tree = gnu::tree<Cafe, gnu::null_type, less<Cafe>, gnu::rb_tree_tag,
                           gnu::tree_order_statistics_node_update>;

// *****

long N, M;
vector<Cafe> cafes;

void read() {
  long X0, Y0, A, B, C, D, E, F;
  cin >> N >> X0 >> Y0 >> A >> B >> C >> D >> E >> F >> M >> ws;
  cafes.resize(N);
  cafes[0] = {int(X0), int(Y0)};
  for (long i = 1; i < N; i++) {
    long x = (A * X0 + B * Y0 + C) % M;
    long y = (D * X0 + E * Y0 + F) % M;
    cafes[i] = {int(x), int(y)};
    X0 = x, Y0 = y;
  }
}

// LOL
auto solve() {
  read();
  sort(cafes.begin(), cafes.end());

  min_tree lhs, rhs;
  for (long i = 0; i < N; i++) {
    rhs.insert({cafes[i].second, i});
  }

  size_t exceptions = 0;
  int start = 0;
  while (start < N) {
    int end = start + 1;
    while (end < N && cafes[start].first == cafes[end].first)
      end++;

    for (long i = start; i < end; i++) {
      rhs.erase({cafes[i].second, i});
    }

    size_t L = start;
    size_t R = N - end;

    for (long i = start; i < end; i++) {
      int y = cafes[i].second;
      size_t q0 = lhs.order_of_key({y, -1});
      size_t q1 = L - lhs.order_of_key({y, N});
      size_t q2 = rhs.order_of_key({y, -1});
      size_t q3 = R - rhs.order_of_key({y, N});
      exceptions += q0 * q3 + q1 * q2;
    }

    for (long i = start; i < end; i++) {
      lhs.insert({cafes[i].second, i});
    }
    start = end;
  }

  size_t choose3 = N * (N - 1) * (N - 2) / 6;
  return choose3 - exceptions;
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
