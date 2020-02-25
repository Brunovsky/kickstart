#include <bits/stdc++.h>

using namespace std;

// *****

static std::random_device rng;
static std::mt19937 mt(rng());

template <typename T>
T random_int(T min = 0, T max = std::numeric_limits<T>::max()) {
  std::uniform_int_distribution<T> dist(min, max);
  return dist(mt);
}

class Solution {
public:
  // find index i in [b..e) such that:
  // if n is in nums[b..e) then
  //   nums[i] == n and i is the largest possible
  // else
  //   nums[i] > n and i is the smallest possible
  size_t latest(const vector<int> &nums, int n, size_t b, size_t e) const {
    auto begin = nums.begin() + b, end = nums.begin() + e;
    auto bound = upper_bound(begin, end, n);
    if (bound - begin > 0 && *(bound - 1) == n)
      return bound - 1 - nums.begin();
    else
      return bound - nums.begin();
  }

  double findMedianSortedArrays(const vector<int> &nums1,
                                const vector<int> &nums2) const {
    const size_t S1 = nums1.size(), S2 = nums2.size();
    const bool odd = (S1 + S2) & 1;
    const size_t S = (S1 + S2 - 1) / 2;

    size_t l1 = 0, r1 = S1; // nums1 window
    size_t l2 = 0, r2 = S2; // nums2 window
    bool nums1_turn = true;

    // sliding and shrinking window pattern, alternating between nums1 and nums2

    // goal: find index l and array N such that numsN[l] is the lowest component
    // of the mean. That means the two arrays can be merged and sorted such that
    // numsN[l] ends up in the (S+1)th position.

    // ends when either window has size 0 or l1 + l2 is near the maximum S.
    while (l1 < r1 && l2 < r2 && l1 + l2 + 1 < S) {
      int N;
      if (nums1_turn) {
        N = nums1[(l1 + r1) / 2];
      } else {
        N = nums2[(l2 + r2) / 2];
      }
      size_t i1 = latest(nums1, N, l1, r1);
      size_t i2 = latest(nums2, N, l2, r2);
      nums1_turn = !nums1_turn;

      // if we tighten the windows from the left, will the sum be too high?
      if (i1 + i2 > S) {
        r1 = i1;
        r2 = i2;
      } else /* i1 + i2 <= S */ {
        l1 = i1;
        l2 = i2;
      }
    }

    // align l1 and l2.
    if (l1 == r1) {
      l2 = S - l1;
    } else if (l2 == r2) {
      l1 = S - l2;
    } else if (l1 + l2 + 1 == S) {
      nums1[l1] < nums2[l2] ? ++l1 : ++l2;
    }

    // care for edge cases where l1 or l2 are at the end of the array.
    if (l2 == S2) {
      return odd ? nums1[l1] : double(nums1[l1] + nums1[l1 + 1]) / 2.0;
    } else if (l1 == S1) {
      return odd ? nums2[l2] : double(nums2[l2] + nums2[l2 + 1]) / 2.0;
    }

    if (odd)
      return min(nums1[l1], nums2[l2]);

    // even case is minimum of 3 values but some may be out of bounds.
    int k11 = INT_MAX, k22 = INT_MAX, k12 = INT_MAX;
    if (l1 < S1 - 1) {
      k11 = nums1[l1] + nums1[l1 + 1];
    }
    if (l2 < S2 - 1) {
      k22 = nums2[l2] + nums2[l2 + 1];
    }
    if (l1 != S1 && l2 != S2) {
      k12 = nums1[l1] + nums2[l2];
    }

    return double(min(k12, min(k11, k22))) / 2.0;
  }

  double test(const vector<int> &nums1, const vector<int> &nums2) const {
    return findMedianSortedArrays(nums1, nums2);
  }
};

// *****

struct Test {
  vector<int> nums1, nums2;
  double expected;
};

Test gen(int t) {
  constexpr int MAX = INT_MAX >> 2;

  Test test;
  int S1 = random_int<int>(0, min(t, MAX));
  int S2 = random_int<int>(1, min(t, MAX));

  vector<int> nums;

  for (int i = 0; i < S1; ++i) {
    test.nums1.push_back(random_int<int>(-10'000, 10'000));
    nums.push_back(test.nums1[i]);
  }

  for (int i = 0; i < S2; ++i) {
    test.nums2.push_back(random_int<int>(-10'000, 10'000));
    nums.push_back(test.nums2[i]);
  }

  sort(test.nums1.begin(), test.nums1.end());
  sort(test.nums2.begin(), test.nums2.end());
  sort(nums.begin(), nums.end());

  int S = nums.size();

  if (S & 1) {
    test.expected = nums[S >> 1];
  } else {
    test.expected = (nums[S >> 1] + nums[(S - 1) >> 1]) / 2.0;
  }

  return test;
}

Test gen2(int t, int k) {
  constexpr int MAX = INT_MAX >> 2;

  Test test;
  int S1 = random_int<int>(0, min(t, MAX));
  int S2 = random_int<int>(1, min(t, MAX));

  vector<int> nums;

  for (int i = 0; i < S1; ++i) {
    for (int j = 0, mj = random_int<int>(1, k); j < mj; ++j) {
      test.nums1.push_back(random_int<int>(-10'000, 10'000));
      nums.push_back(test.nums1.back());
    }
  }

  for (int i = 0; i < S2; ++i) {
    for (int j = 0, mj = random_int<int>(1, k); j < mj; ++j) {
      test.nums2.push_back(random_int<int>(-10'000, 10'000));
      nums.push_back(test.nums2.back());
    }
  }

  sort(test.nums1.begin(), test.nums1.end());
  sort(test.nums2.begin(), test.nums2.end());
  sort(nums.begin(), nums.end());

  int S = nums.size();

  if (S & 1) {
    test.expected = nums[S >> 1];
  } else {
    test.expected = (nums[S >> 1] + nums[(S - 1) >> 1]) / 2.0;
  }

  return test;
}

void test() {
  Solution S;

  for (int t = 1; t <= 50; ++t) {
    Test test = gen(t);
    double result = S.test(test.nums1, test.nums2);

    if (result != test.expected) {
      cout << "Failed test " << t << "\n";
      cout << result << " VS " << test.expected << '\n';
      return;
    }

    cout << "\rTest " << t << flush;
  }

  cout << '\n';

  for (int t = 1; t <= 20; ++t) {
    for (int k = 4; k < 10; ++k) {
      Test test = gen2(t, k);
      double result = S.test(test.nums1, test.nums2);

      if (result != test.expected) {
        cout << "Failed test " << t << "\n";
        cout << result << " VS " << test.expected << '\n';
        return;
      }

      cout << "\rTest " << t << flush;
    }
  }

  cout << '\n'
       << S.test({1, 1, 1, 9, 9, 9}, {3, 4, 5, 6, 7, 8}) << '\n'
       << S.test({1, 1, 1, 3, 5, 9}, {3, 4, 6, 6, 7, 8}) << '\n'
       << S.test({2, 3, 3, 3, 7, 7, 10}, {4, 4, 5, 5, 5, 5, 6}) << '\n'
       << S.test({1, 2, 3, 7, 10}, {4, 5, 6, 8, 9}) << '\n';

  // 5.5, 4.5, 5, 5.5

  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
