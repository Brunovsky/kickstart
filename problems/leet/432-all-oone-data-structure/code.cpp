#include <bits/stdc++.h>

using namespace std;

// *****

struct list_node_t {
  int value;
  list<string> keys;
};

struct map_node_t {
  list<list_node_t>::iterator node;
  list<string>::iterator key_it;
};

class AllOne {
  list<list_node_t> nodes;
  unordered_map<string, map_node_t> map;

public:
  /** Initialize your data structure here. */
  AllOne() {
    nodes.push_front({1, {}});
  }

  /** Inserts a new key <Key> with value 1. Or increments an existing key by 1.
   */
  void inc(string key) {
    auto mit = map.find(key);

    // insert key
    if (mit == map.end()) {
      // the head of nodes contains value 1
      auto nit = nodes.begin();
      nit->keys.push_front(key);
      map.emplace(key, map_node_t{nodes.begin(), nit->keys.begin()});
      return;
    }

    auto &mnode = mit->second;
    int new_value = mnode.node->value + 1;

    // change from list node *lit to *nit
    auto lit = mnode.node;
    auto nit = next(lit);

    if (nit == nodes.end() || nit->value != new_value) {
      nit = nodes.insert(nit, {new_value, {}});
    }

    nit->keys.splice(nit->keys.begin(), lit->keys, mnode.key_it);

    if (lit->keys.empty() && lit->value > 1) {
      nodes.erase(lit);
    }

    mit->second.node = nit;
    mit->second.key_it = nit->keys.begin();
  }

  /** Decrements an existing key by 1. If Key's value is 1, remove it from the
   * data structure. */
  void dec(string key) {
    auto mit = map.find(key);

    if (mit == map.end()) {
      return;
    }

    auto &mnode = mit->second;
    int new_value = mnode.node->value - 1;

    // remove key
    if (new_value == 0) {
      mnode.node->keys.erase(mnode.key_it);
      map.erase(mit);
      return;
    }

    // change from list node *nit to *lit
    auto nit = mnode.node;
    auto lit = prev(nit);

    if (lit->value != new_value) {
      lit = nodes.insert(nit, {new_value, {}});
    }

    lit->keys.splice(lit->keys.begin(), nit->keys, mnode.key_it);

    if (nit->keys.empty()) {
      nodes.erase(nit);
    }

    mit->second.node = lit;
    mit->second.key_it = lit->keys.begin();
  }

  /** Returns one of the keys with maximal value. */
  string getMaxKey() const {
    if (map.empty()) {
      return "";
    } else {
      return nodes.back().keys.front();
    }
  }

  /** Returns one of the keys with Minimal value. */
  string getMinKey() const {
    if (map.empty()) {
      return "";
    } else if (nodes.front().keys.empty()) {
      return next(nodes.begin())->keys.front();
    } else {
      return nodes.front().keys.front();
    }
  }
};

/**
 * Your AllOne object will be instantiated and called as such:
 * AllOne* obj = new AllOne();
 * obj->inc(key);
 * obj->dec(key);
 * string param_3 = obj->getMaxKey();
 * string param_4 = obj->getMinKey();
 */

// *****

void minmax(const AllOne &ds) {
  printf("(%s, %s)\n", ds.getMinKey().c_str(), ds.getMaxKey().c_str());
}

int main() {
  AllOne ds;
  ds.inc("A"); // A=1
  minmax(ds);
  ds.inc("A"); // A=2
  ds.inc("A"); // A=3
  minmax(ds);
  ds.inc("B"); // B=1
  minmax(ds);
  ds.dec("B");
  minmax(ds);
  ds.inc("C"); // C=1
  ds.inc("C"); // C=2
  ds.inc("C"); // C=3
  minmax(ds);
  ds.inc("C"); // C=4
  minmax(ds);
  return 0;
}
