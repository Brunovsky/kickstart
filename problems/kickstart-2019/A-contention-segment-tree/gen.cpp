#include <bits/stdc++.h>

using namespace std;

using u32 = uint32_t;

mt19937 rng(0x97439281);

int main() {
  ofstream file("lol.txt");
  file << "800000 300" << '\n';

  uniform_int_distribution<u32> dist(0, 9);

  for (u32 i = 0; i < 300; ++i) {
    u32 l = 350'000 - 10 * i - dist(rng);
    u32 r = 350'050 + 10 * i + dist(rng);
    file << l << ' ' << r << '\n';
  }

  return 0;
}
