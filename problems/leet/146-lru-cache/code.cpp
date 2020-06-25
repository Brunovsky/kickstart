#include <bits/stdc++.h>

using namespace std;

// *****

using list_pt = list<int>::iterator;

class LRUCache {
  list<int> keys;
  unordered_map<int, pair<list_pt, int>> cache;
  uint capacity;

public:
  LRUCache(int capacity) : capacity(capacity) {}

  int get(int key) {
    auto it = cache.find(key); // amortized O(1)
    if (it == cache.end()) {
      return -1;
    }
    keys.erase(it->second.first);
    keys.push_front(key);
    it->second.first = keys.begin();
    return it->second.second;
  }

  void put(int key, int value) {
    auto it = cache.find(key); // amortized O(1)
    if (it == cache.end()) {
      if (keys.size() == capacity) {
        int old_key = keys.back();
        keys.pop_back();
        cache.erase(old_key);
      }
      keys.push_front(key);
      cache[key] = {keys.begin(), value}; // amortized O(1)
    } else {
      keys.erase(it->second.first);
      keys.push_front(key);
      it->second = {keys.begin(), value};
    }
  }
};

// *****

int main() {
  return 0;
}
