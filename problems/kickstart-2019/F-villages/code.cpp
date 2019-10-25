#include <bits/stdc++.h>

using namespace std;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// *****

struct Score {
  i64 N = 0;
  i64 R = 0;
  i64 P = 0;
};

u32 V;
vector<i64> B;           // beauty
vector<vector<u32>> nb;  // neighbours
vector<Score> S;         // scores

void visit(u32 i, u32 parent) {
  for (u32 j : nb[i]) {
    if (j == parent) continue;
    visit(j, i);
  }

  S[i].N = 0;
  S[i].R = B[i];
  S[i].P = B[i];

  i64 nop = 0, yes = 0;
  bool first = true;

  for (u32 j : nb[i]) {
    if (j == parent) continue;

    S[i].R += S[j].P;
    S[i].P += max(S[j].N, S[j].R);

    if (first) {
      yes = S[j].R;
      nop = S[j].N;
      first = false;
    } else {
      yes = max(max(S[j].N, S[j].R) + yes, S[j].R + nop);
      nop = nop + S[j].N;
    }
  }

  if (!first) S[i].N = max(nop, yes + B[i]);
}

auto solve() {
  visit(0, 0);
  return max(S[0].N, S[0].R);
}

// *****

void parse_test() {
  cin >> V >> ws;
  B.resize(V, 0);
  nb.resize(V);
  S.resize(V);
  for (u32 i = 0; i < V; ++i) cin >> B[i];
  for (u32 i = 0; i < V - 1; ++i) {
    u32 Xi, Yi;
    cin >> Xi >> Yi >> ws;
    --Xi, --Yi;
    nb[Xi].push_back(Yi);
    nb[Yi].push_back(Xi);
  }
}

void reset_test() {
  B.clear();
  nb.clear();
  S.clear();
}

void print_test(u64 casenum, i64 solution) {
  cout << "Case #" << casenum << ": ";
  cout << solution;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  u64 T;
  cin >> T >> ws;
  for (u64 t = 0; t < T; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
