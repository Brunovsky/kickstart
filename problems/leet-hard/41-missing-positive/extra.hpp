#include <bits/stdc++.h>

// * Simple numerics
// Absolute value  midpoint  absolute difference  distance

template <typename N>
constexpr inline N abs(N num) noexcept {
  return num > N(0) ? num : -num;
}

template <typename N>
constexpr inline N distance(N n, N m) noexcept {
  return n < m ? m - n : n - m;
}

template <typename N>
constexpr inline N midpoint(N n, N m) noexcept {
  return n < m ? n + (m - n) / 2 : m + (n - m) / 2;
}

// * GCD, LCM polyfills for C++17
// GCD  LCM  MDC  MMC

namespace detail {

template <typename Mn, typename Nn>
constexpr std::common_type_t<Mn, Nn> gcd(Mn m, Nn n) {
  return m == 0 ? abs(n) : n == 0 ? abs(m) : detail::gcd(n, m % n);
}

template <typename Mn, typename Nn>
constexpr std::common_type_t<Mn, Nn> lcm(Mn m, Nn n) {
  return (m != 0 && n != 0) ? (abs(m) / detail::gcd(m, n)) * abs(n) : 0;
}

}  // namespace detail

template <typename Mn, typename Nn>
constexpr inline std::common_type_t<Mn, Nn> gcd(Mn m, Nn n) {
  return detail::gcd(m, n);
}

template <typename Mn, typename Nn>
constexpr inline std::common_type_t<Mn, Nn> lcm(Mn m, Nn n) {
  return detail::lcm(m, n);
}

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
  return dist(mt);
}

template <typename T>
T random_real(T min = 0, T max = std::numeric_limits<T>::max()) {
  std::uniform_real_distribution<T> dist(min, max);
  return dist(mt);
}

// * Thread pool
// Pool  workers

class thread_pool {
  using Job = std::function<void()>;
  using Mutex = std::mutex;

  std::vector<std::thread> threads;
  std::list<Job> jobqueue;
  mutable Mutex mutex;
  std::condition_variable cv;

  bool closed_queue = false;
  bool joined = false, detached = false;

 private:
  inline bool worker_break() const noexcept {
    return closed_queue;  // && (joined || detached), but this is implied
  }

  void worker_loop(size_t id) {
    while (true) {
      std::unique_lock<Mutex> guard(mutex);

      if (jobqueue.empty()) {
        cv.wait(guard, [this]() { return !jobqueue.empty() || worker_break(); });
      }

      if (jobqueue.empty() && worker_break()) break;

      auto job = jobqueue.front();
      jobqueue.pop_front();
      guard.unlock();
      job();
    }

    // index in the thread vector, unused
    (void)id;
  }

 public:
  explicit thread_pool(u16 nthreads) {
    assert(nthreads > 0);
    for (u64 id = 0; id < nthreads; ++id) {
      threads.emplace_back([this, id]() { worker_loop(id); });
    }
  }

  thread_pool(const thread_pool&) = delete;
  thread_pool& operator=(const thread_pool&) = delete;

  inline size_t pool_size() const noexcept {
    return threads.size();
  }

  inline bool joinable() const noexcept {
    return !closed_queue;
  }

  void join() {
    std::unique_lock<Mutex> guard(mutex);
    assert(joinable());

    closed_queue = true;
    joined = true;

    cv.notify_all();
    guard.unlock();
    for (auto& thread : threads) thread.join();
  }

  void detach() {
    std::unique_lock<Mutex> guard(mutex);
    assert(joinable());

    closed_queue = true;
    detached = true;

    cv.notify_all();
    guard.unlock();
    for (auto& thread : threads) thread.detach();
  }

  void submit(Job job) {
    std::unique_lock<Mutex> guard(mutex);
    assert(joinable());

    jobqueue.push_back(std::move(job));
    cv.notify_one();
  }
};
