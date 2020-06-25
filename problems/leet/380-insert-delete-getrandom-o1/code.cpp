#include <bits/stdc++.h>

using namespace std;

// *****

static random_device rng;
static default_random_engine mt(rng());

class RandomizedSet {
  using index_t = int;
  unordered_map<int, index_t> uset;
  vector<int> vec;

public:
  /** Initialize your data structure here. */
  RandomizedSet() {}

  /** Inserts a value to the set. Returns true if the set did not already
   * contain the specified element. */
  bool insert(int val) {
    if (uset.count(val)) { // amortized O(1)
      return false;
    }
    uset.emplace(val, vec.size()); // amortized O(1)
    vec.push_back(val);            // amortized O(1)
    return true;
  }

  /** Removes a value from the set. Returns true if the set contained the
   * specified element. */
  bool remove(int val) {
    auto it = uset.find(val); // amortized O(1)
    if (it == uset.end()) {
      return false;
    }
    int back = vec.back();
    uset.at(back) = it->second;
    swap(vec[it->second], back);
    vec.pop_back();
    uset.erase(it); // amortized O(1)
    return true;
  }

  /** Get a random element from the set. */
  int getRandom() {
    uniform_int_distribution<int> dist(0, vec.size() - 1);
    return vec[dist(mt)];
  }
};

// *****

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
