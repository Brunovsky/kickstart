#include <bits/stdc++.h>

using namespace std;

using i64 = int64_t;

// ***** ***** ***** ***** ***** Knuth Morris Pratt (from library)

namespace {

class knuth_morris_pratt {
  std::vector<int> lookup_;
  std::string pattern;

 public:
  knuth_morris_pratt(std::string pattern);
  int lookup(std::size_t index) const;
  std::size_t shift(std::size_t index) const;
  const std::string& get_pattern() const;
};

knuth_morris_pratt::knuth_morris_pratt(std::string pattern) : pattern(pattern) {
  std::size_t P = pattern.size();

  lookup_.resize(P + 1);

  lookup_[0] = -1;
  long border = 0;  // need to compare against 0

  for (std::size_t index = 1; index < P; ++index, ++border) {
    if (pattern[index] == pattern[border]) {
      lookup_[index] = lookup_[border];
    } else {
      lookup_[index] = border;

      do {
        border = lookup_[border];
      } while (border >= 0 && pattern[index] != pattern[border]);
    }
  }

  lookup_[P] = border;
}

int knuth_morris_pratt::lookup(std::size_t index) const {
  return lookup_[index];
}

std::size_t knuth_morris_pratt::shift(std::size_t index) const {
  return index - lookup_[index];
}

const std::string& knuth_morris_pratt::get_pattern() const {
  return pattern;
}

std::vector<std::size_t> knuth_morris_pratt_search(const std::string& text,
                                                   const knuth_morris_pratt& kmp) {
  const std::string& pattern = kmp.get_pattern();
  long P = pattern.size(), T = text.size();

  std::vector<std::size_t> match;
  if (P == 0) return match;

  long i = 0;
  long j = 0;

  while (i <= T - P) {
    while ((j < P) && (text[i + j] == pattern[j])) ++j;

    if (j == P) {
      // Matched
      match.push_back(i);
      i += kmp.shift(P);
      j = kmp.lookup(P);
    } else {
      // Mismatched
      i += kmp.shift(j);
      j = kmp.lookup(j);
    }

    if (j < 0) j = 0;
  }

  return match;
}

}  // namespace

// ***** ***** ***** ***** *****

// *****

int R, C;
vector<string> text_rows;
vector<string> text_cols;
vector<string> words;
i64 H[102][102][101], V[102][102][101];

void fill_word_counts(const string& word) {
  if (word.empty()) return;

  knuth_morris_pratt kmp(word);
  const int l = word.size();

  for (int r = 1; r <= R; ++r) {
    auto indices = knuth_morris_pratt_search(text_rows[r - 1], kmp);
    for (int c : indices) H[r][c + l][l] += i64(l);
  }

  for (int c = 1; c <= C; ++c) {
    auto indices = knuth_morris_pratt_search(text_cols[c - 1], kmp);
    for (int r : indices) V[r + l][c][l] += i64(l);
  }
}

auto solve() {
  memset(H, 0, sizeof(H));
  memset(V, 0, sizeof(V));

  for (string word : words) {
    fill_word_counts(word);
    reverse(word.begin(), word.end());
    fill_word_counts(word);
  }

  for (int r = 1; r <= R; ++r) {
    for (int c = 1; c <= C; ++c) {
      for (int l = 2; l <= C; ++l) {
        H[r][c][l] += H[r][c][l - 1];  // prefix sum lengths
      }
      for (int l = 2; l <= R; ++l) {
        V[r][c][l] += V[r][c][l - 1];
      }
    }
  }

  for (int r = 1; r <= R; ++r) {
    for (int c = 1; c <= C; ++c) {
      for (int l = 2; l <= C; ++l) {
        H[r][c][l] += H[r][c - 1][l - 1];  // [...|] --> [... ]
      }
      for (int l = 2; l <= R; ++l) {
        V[r][c][l] += V[r - 1][c][l - 1];
      }
    }
  }

  for (int r = 1; r <= R; ++r) {
    for (int c = 1; c <= C; ++c) {
      for (int l = 1; l <= C; ++l) {
        H[r][c][l] += H[r - 1][c][l];  // prefix prefix sum
      }
      for (int l = 1; l <= R; ++l) {
        V[r][c][l] += V[r][c - 1][l];
      }
    }
  }

  i64 best_len = 0, best_sum = R + C;
  i64 count = 0;
  // best == best_len / best_sum

  for (int ra = 0; ra < R; ++ra) {
    for (int rb = ra + 1; rb <= R; ++rb) {
      for (int ca = 0; ca < C; ++ca) {
        for (int cb = ca + 1; cb <= C; ++cb) {
          i64 lh = cb - ca;
          i64 lv = rb - ra;
          i64 h = H[rb][cb][lh] - H[ra][cb][lh];
          i64 v = V[rb][cb][lv] - V[rb][ca][lv];
          i64 len = h + v;
          i64 sum = lh + lv;
          // best_len / best_sum <= len / sum
          i64 difference = best_len * sum - len * best_sum;
          if (difference < 0) {
            best_len = len;
            best_sum = sum;
            count = 1;
          } else if (difference == 0) {
            ++count;
          }
        }
      }
    }
  }

  // poor man's gcd
  for (i64 i = best_sum; i > 0; --i) {
    if ((best_len % i) == 0 && (best_sum % i) == 0) {
      best_len /= i;
      best_sum /= i;
      break;
    }
  }

  return to_string(best_len) + "/" + to_string(best_sum) + " " + to_string(count);
}

// *****

void reparse_test() {
  int W;
  cin >> R >> C >> W >> ws;
  text_rows.assign(R, {});
  text_cols.assign(C, {});
  for (int r = 0; r < R; ++r) {
    string text;
    cin >> text >> ws;
    assert(text.size() == size_t(C));
    text_rows[r] = move(text);
  }
  for (int c = 0; c < C; ++c) {
    text_cols[c].resize(R);
    for (int r = 0; r < R; ++r) {
      text_cols[c][r] = text_rows[r][c];
    }
  }
  words.clear();
  for (int w = 0; w < W; ++w) {
    string word;
    cin >> word;
    int len = word.size();
    if (len > R && len > C) continue;
    words.push_back(move(word));
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
