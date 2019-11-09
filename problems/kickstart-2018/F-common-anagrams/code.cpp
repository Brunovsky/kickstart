#include <bits/stdc++.h>

using namespace std;

// *****

#define ASCII ('Z' - 'A' + 1)

using alph = array<int, ASCII>;

alph sub(const alph& lhs, const alph& rhs) {
  alph res;
  for (int i = 0; i < ASCII; ++i) res[i] = lhs[i] - rhs[i];
  return res;
}

int L;
string A, B;
vector<alph> pA, pB;

auto solve() {
  pA.assign(L + 1, {});
  pB.assign(L + 1, {});
  pA[0] = {};
  pB[0] = {};

  size_t numtuples = 0;

  for (int i = 1; i <= L; ++i) {
    pA[i] = pA[i - 1];
    pB[i] = pB[i - 1];
    ++pA[i][A[i - 1] - 'A'];
    ++pB[i][B[i - 1] - 'A'];
  }

  for (int len = 1; len <= L; ++len) {
    size_t num = L - len + 1;

    vector<alph> sA(num), sB(num);

    for (int i = 0, j = len; j <= L; ++i, ++j) {
      sA[i] = sub(pA[j], pA[i]);
      sB[i] = sub(pB[j], pB[i]);
    }

    sort(sA.begin(), sA.end());
    sort(sB.begin(), sB.end());

    size_t a = 0, b = 0;
    while (a < num && b < num) {
      if (sA[a] == sB[b]) {
        ++numtuples;
        ++a;
      } else if (sA[a] < sB[b]) {
        ++a;
      } else {
        ++b;
      }
    }
  }

  return numtuples;
}

// *****

void reparse_test() {
  A.clear();
  B.clear();
  cin >> L >> ws >> A >> ws >> B >> ws;
  assert(A.length() == size_t(L) && B.length() == size_t(L));
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 0; t < T; ++t) {
    reparse_test();
    cout << "Case #" << (t + 1) << ": " << solve() << '\n';
  }
  return 0;
}
