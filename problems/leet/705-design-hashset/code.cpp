#include <bits/stdc++.h>

using namespace std;

// *****

class MyHashSet {
  vector<int> buckets[4096]{};

  size_t hash(size_t n) const {
    return (n * n + 7) & 0xfff;
  }

  auto find(size_t h, int key) const {
    return std::find(begin(buckets[h]), end(buckets[h]), key);
  }

public:
  MyHashSet() {}

  void add(int key) {
    size_t h = hash(key);
    auto it = find(h, key);
    if (it == buckets[h].end()) {
      buckets[h].push_back(key);
    }
  }

  void remove(int key) {
    size_t h = hash(key);
    auto it = find(h, key);
    if (it != buckets[h].end()) {
      buckets[h].erase(it);
    }
  }

  bool contains(int key) {
    size_t h = hash(key);
    return find(h, key) != buckets[h].end();
  }
};

// *****

int main() {
  return 0;
}
