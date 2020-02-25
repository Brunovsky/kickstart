#include <bits/stdc++.h>

using namespace std;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// *****

using SkillSet = vector<u16>;

namespace std {

template <>
struct hash<vector<u16>> {
  inline size_t operator()(const SkillSet &vec) const noexcept {
    size_t seed = vec.size();
    for (auto i : vec) {
      seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

} // namespace std

inline ostream &operator<<(ostream &out, const SkillSet &skillset) {
  out << "[" << skillset[0];
  for (size_t i = 1; i < skillset.size(); ++i)
    out << ", " << skillset[i];
  out << "]";
  return out;
}

struct Info {
  SkillSet skillset;
  u32 employees = 0;
  vector<u32> subsets{};
};

vector<Info> infos;
unordered_map<SkillSet, u32> finger;
u64 N, S;

u64 solve() {
  u64 exceptions = 0;

  for (u32 i = 1; i < infos.size(); ++i) {
    const auto &info = infos[i];
    if (info.employees == 0)
      continue;

    u32 underlyings = 0;
    for (u32 index : info.subsets)
      underlyings += infos[index].employees;

    exceptions += info.employees * (info.employees - 1 + underlyings);
  }

  return N * (N - 1) - exceptions;
}

u32 add_skillset(SkillSet &skillset) {
  Info info{skillset, 0};
  info.subsets.reserve(1 << skillset.size());

  if (skillset.size() > 1) {
    for (u16 C = skillset.size(), c = 0; c < C; ++c) {
      SkillSet subset = skillset;
      subset.erase(subset.begin() + c);
      u32 subset_index = finger[subset];
      if (subset_index == 0) {
        subset_index = add_skillset(subset);
      }
      info.subsets.push_back(subset_index);
      info.subsets.insert(info.subsets.end(),
                          infos[subset_index].subsets.begin(),
                          infos[subset_index].subsets.end());
    }

    sort(info.subsets.begin(), info.subsets.end());
    auto nend = unique(info.subsets.begin(), info.subsets.end());
    info.subsets.erase(nend, info.subsets.end());
    info.subsets.shrink_to_fit();
  }

  u32 index = infos.size();
  finger[skillset] = index;
  infos.push_back(move(info));
  return index;
}

// *****

void reparse_test() {
  cin >> N >> S;
  infos.clear();
  finger.clear();
  infos.resize(1);

  for (u64 n = 0; n < N; ++n) {
    u64 C;
    cin >> C;

    SkillSet skillset(C);

    for (u16 c = 0; c < C; ++c) {
      cin >> skillset[c];
    }

    sort(skillset.begin(), skillset.end());
    u32 index = finger[skillset];

    if (index == 0) {
      index = add_skillset(skillset);
    }

    ++infos[index].employees;
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
