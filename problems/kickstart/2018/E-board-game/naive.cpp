#include <bits/stdc++.h>

using namespace std;

// *****

// (3N choose N)
constexpr size_t choose2N[] = {1, 2, 6, 20, 70, 252};
constexpr size_t choose3N[] = {1, 3, 15, 84, 495, 3003};

#define SUP1MASK (1 << (3 * N))
#define SUP2MASK (1 << (2 * N))
#define NMASK ((1 << N) - 1)

int N;
int Asum, Bsum;
vector<int> A;
vector<int> B;

inline int next_lexicographical_mask(int v) {
  int t = v | (v - 1);
  int w = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
  assert(__builtin_popcount(v) == N);
  return w;
}

// O(3N)
int sum_mask(int mask, const vector<int> &V) {
  int sum = 0;
  for (int i = 0; i < 3 * N; i++) {
    if (mask & (1 << i)) {
      sum += V[i];
    }
  }
  return sum;
}

// O(3N)
auto remaining_cards(int mask, const vector<int> &V) {
  vector<int> remaining;
  remaining.reserve(2 * N);
  for (int i = 0; i < 3 * N; i++) {
    if (!(mask & (1 << i))) {
      remaining.push_back(V[i]);
    }
  }
  return remaining;
}

void read() {
  cin >> N >> ws;
  A.resize(3 * N + 1);
  B.resize(3 * N + 1);
  for (int i = 0; i < 3 * N; i++) {
    cin >> B[i] >> ws;
  }
  for (int i = 0; i < 3 * N; i++) {
    cin >> A[i] >> ws;
  }
  A[3 * N] = B[3 * N] = 0; // to allow masks larger than 1 << 3N
  Asum = accumulate(A.begin(), A.end(), 0);
  Bsum = accumulate(B.begin(), B.end(), 0);
}

auto solve() {
  read();
  int max_beaten = 0;

  // Iterate (B1, B2, B3) sums
  int b1mask = NMASK;
  do {
    int B1 = sum_mask(b1mask, B);
    auto Bremaining = remaining_cards(b1mask, B);
    b1mask = next_lexicographical_mask(b1mask);

    int b2mask = NMASK;
    do {
      int B2 = sum_mask(b2mask, Bremaining);
      int B3 = Bsum - (B1 + B2);
      b2mask = next_lexicographical_mask(b2mask);

      if (B1 < B2 || B2 < B3) {
        continue;
      }

      int beaten = 0;

      // Iterate (A1, A2, A3) sums
      int a1mask = NMASK;
      int count = 0;
      do {
        int A1 = sum_mask(a1mask, A);
        auto Aremaining = remaining_cards(a1mask, A);
        a1mask = next_lexicographical_mask(a1mask);

        int a2mask = NMASK;
        do {
          int A2 = sum_mask(a2mask, Aremaining);
          int A3 = Asum - (A1 + A2);
          a2mask = next_lexicographical_mask(a2mask);

          bool c1 = A1 < B1;
          bool c2 = A2 < B2;
          bool c3 = A3 < B3;
          beaten += (c1 && c2) || (c1 && c3) || (c2 && c3);

        } while (a2mask < SUP2MASK);
      } while (a1mask < SUP1MASK);
      max_beaten = max(max_beaten, beaten);
    } while (b2mask < SUP2MASK);
  } while (b1mask < SUP1MASK);

  return max_beaten / double(choose3N[N] * choose2N[N]);
}
// *****

int main() {
  unsigned T;
  cout << fixed << showpoint << setprecision(8);
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
