#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;
using u64 = uint64_t;

// *****

using abc_t = array<u32, 26>;
using wordset_t = array<array<vector<abc_t>, 26>, 26>;

u32 L, N;
string text;
map<u32, wordset_t> words;

auto solve() {
  u32 solutions = 0;

  for (auto &p : words) {
    u32 len = p.first;
    auto &wordset = p.second;

    if (len > N)
      continue;

    //    u32 remaining = 0;
    //    for (u32 l = 0; l < 26; ++l)
    //      for (u32 r = 0; r < 26; ++r) remaining += wordset[l][r].size();

    abc_t abc{};
    u32 li = 0, ri = len - 1;

    for (u32 i = li + 1; i < ri; ++i)
      ++abc[text[i]];

    while (ri < N) {
      auto &vec = wordset[text[li]][text[ri]];

      // cout << "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n";
      // for (u32 i = 0; i < 26; ++i) cout << abc[i] << ' ';
      // cout << '\n' << text << endl;
      // cout << string(li, ' ') << '^' << string(len - 2, ' ') << "^\n" <<
      // endl;

      auto lend = remove(vec.begin(), vec.end(), abc);
      solutions += vec.end() - lend;
      // remaining -= vec.end() - lend;
      vec.erase(lend, vec.end());

      ++abc[text[ri++]];
      --abc[text[++li]];
    }
  }

  return solutions;
}

// *****

void reparse_test() {
  cin >> L >> ws;

  words.clear();
  text.clear();

  for (u32 i = 0; i < L; ++i) {
    string text;
    cin >> text >> ws;

    size_t len = text.length();
    char l = text.front() - 'a', r = text.back() - 'a';
    text.pop_back();

    abc_t abc{};

    for (char c : text)
      ++abc[c - 'a'];
    --abc[text[0] - 'a'];

    words[len][l][r].push_back(abc);
  }

  u64 A, B, C, D;
  char s1, s2;
  cin >> ws >> s1 >> ws >> s2 >> N >> A >> B >> C >> D >> ws;

  text.resize(N);
  text[0] = s1 - 'a';
  text[1] = s2 - 'a';

  u64 x1 = u64(s1), x2 = u64(s2);
  u64 xi_1 = x2, xi_2 = x1;

  for (u32 i = 2; i < N; ++i) {
    u64 xi = (A * xi_1 + B * xi_2 + C) % D;
    text[i] = char(xi % 26);
    xi_2 = xi_1;
    xi_1 = xi;
  }

  cout << "A=" << A << " B=" << B << " C=" << C << " D=" << D << '\n';
  cout << "text: ";
  for (char c : text)
    cout << char(c + 'a');
  cout << '\n';
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
