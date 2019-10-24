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

struct Slot {
  u64 cod, eat;
};

struct Day {
  u64 coding, eating;
  bool verdict = false;
  u32 i;
};

u32 D, S;
vector<Slot> slots;
vector<Day> days;

// C0  C1  C2  ... Cs  Cs+1 ...  CS-1
// E0  E1  E2  ... Es  Es+1 ...  ES-1
//

auto solve() {
  // Sort the days by coding requirement, increasing.
  sort(days.begin(), days.end(),
       [](const Day& d1, const Day& d2) { return d1.coding < d2.coding; });

  // Sort the slots in such a way that the ratio cod/eat is decreasing.
  sort(slots.begin(), slots.end(),
       [](const Slot& s1, const Slot& s2) { return s1.cod * s2.eat > s1.eat * s2.cod; });

  u64 cod = 0, eat = 0;

  for (u32 s = 0; s < S; ++s) {
    eat += slots[s].eat;
  }

  for (u32 d = 0, s = 0; d < D; ++d) {
    // transform next slot fully into coding?
    while (s < S && cod + slots[s].cod <= days[d].coding) {
      cod += slots[s].cod;
      eat -= slots[s++].eat;
    }

    if (s == S) {
      if (cod < days[d].coding) break;
      days[d].verdict = eat >= days[d].eating;
      continue;
    }

    // invariant: cod <= days[d].coding < cod + slots[s].cod

    if (eat < days[d].eating) continue;  // exit early if impossible
    u64 codlo = days[d].coding - cod, codrange = slots[s].cod;

    // f = codlo / slots[s].cod
    // decrease in day.eating: f * slots[s].eat = codlo / slots[s].cod * slots[s].eat
    // we must have:

    // day.eating <= eat - f * slots[s].eat
    // f * slots[s].eat <= eat - day.eating
    // codlo * slots[s].eat <= (eat - day.eating) * slots[s].cod

    days[d].verdict = codlo * slots[s].eat <= (eat - days[d].eating) * slots[s].cod;
  }

  // this could be O(D) but I'm lazy
  sort(days.begin(), days.end(),
       [](const Day& d1, const Day& d2) { return d1.i < d2.i; });

  std::string text;
  text.resize(D);
  for (u32 d = 0; d < D; ++d) text[d] = days[d].verdict ? 'Y' : 'N';

  return text;
}

// *****

void parse_test() {
  cin >> D >> S;
  slots.resize(S);
  days.resize(D);
  for (u32 s = 0; s < S; ++s) {
    cin >> slots[s].cod >> slots[s].eat >> ws;
  }
  for (u32 d = 0; d < D; ++d) {
    cin >> days[d].coding >> days[d].eating >> ws;
    days[d].i = d;
    days[d].verdict = false;
  }
}

void reset_test() {
  D = S = 0;
  slots.clear();
  days.clear();
}

void print_test(u64 casenum, string solution) {
  cout << "Case #" << casenum << ": ";
  cout << solution;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  u64 T;
  cin >> T >> ws;
  for (u64 t = 0; t < T; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
