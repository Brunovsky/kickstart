#include <bits/stdc++.h>

using namespace std;

// *****

struct frequency_list_t {
  int frequency;
  list<int> keys;
};

struct map_node_t {
  list<frequency_list_t>::iterator fnode;
  list<int>::iterator key_it;
  int value;
};

class LFUCache {
  list<frequency_list_t> linked;
  unordered_map<int, map_node_t> map;

  int capacity;

public:
  LFUCache(int capacity) : capacity(capacity) {}

  int get(int key) {
    auto mit = map.find(key);

    if (mit == map.end()) {
      return -1;
    }

    auto &mnode = mit->second;
    update_frequency(mnode);
    return mnode.value;
  }

  void put(int key, int value) {
    auto mit = map.find(key);

    if (mit == map.end()) {
      if (capacity == 0) {
        return;
      }
      evict();

      auto fnode = linked.begin();
      if (fnode->frequency != 1) {
        fnode = linked.insert(fnode, {1, {}});
      }
      fnode->keys.push_front(key);

      map_node_t mnode = {fnode, fnode->keys.begin(), value};
      map.emplace(key, mnode);
      return;
    }

    auto &mnode = mit->second;
    update_frequency(mnode);
    mnode.value = value;
  }

  void evict() {
    if (int(map.size()) < capacity) {
      return;
    }
    auto key = linked.front().keys.back();
    map.erase(key);
    linked.front().keys.pop_back();
    if (linked.front().keys.empty()) {
      linked.pop_front();
    }
  }

  void update_frequency(map_node_t &mnode) {
    int new_frequency = mnode.fnode->frequency + 1;

    auto lit = mnode.fnode;
    auto nit = next(lit);

    if (nit == linked.end() || nit->frequency != new_frequency) {
      nit = linked.insert(nit, {new_frequency, {}});
    }

    nit->keys.splice(nit->keys.begin(), lit->keys, mnode.key_it);

    if (lit->keys.empty()) {
      linked.erase(lit);
    }

    mnode.fnode = nit;
    mnode.key_it = nit->keys.begin();
  }
};

// *****

int main() {
  return 0;
}
