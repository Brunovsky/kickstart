#include <bits/stdc++.h>

using namespace std;

// * Random
// Random  rng  shuffle  random_shuffle  distribution

std::random_device default_random_device;
std::mt19937 rng(default_random_device());

// shuffle C++17 polyfill
template <class RandomIt, class URBG>
void shuffle(RandomIt first, RandomIt last, URBG&& g = rng) {
  typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;
  typedef std::uniform_int_distribution<diff_t> distr_t;
  typedef typename distr_t::param_type param_t;

  distr_t D;
  diff_t n = last - first;
  for (diff_t i = n - 1; i > 0; --i) {
    using std::swap;
    swap(first[i], first[D(g, param_t(0, i))]);
  }
}
template <typename T>
T random_int(T min = 0, T max = std::numeric_limits<T>::max()) {
  std::uniform_int_distribution<T> dist(min, max);
  return dist(rng);
}

int main() {
  ofstream file("test.txt");
  file << 100 << '\n';

  for (int i = 0; i < 100; ++i) {
    uint64_t L = random_int<uint64_t>(0UL, 1'000'000'000UL);
    uint64_t R = random_int<uint64_t>(L, L + 100'000UL);
    file << L << ' ' << R << '\n';
  }

  return 0;
}
