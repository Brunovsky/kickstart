#include <bits/stdc++.h>

using namespace std;

// *****

class FooBar {
private:
  int n;
  mutex mtx;
  condition_variable cv;
  int k = 0;

public:
  FooBar(int n) {
    this->n = n;
  }

  void foo(function<void()> printFoo) {
    unique_lock<mutex> guard(mtx);

    for (int i = 0; i < n; i++) {
      cv.wait(guard, [i, this]() { return this->k >= 2 * i; });

      // printFoo() outputs "foo". Do not change or remove this line.
      printFoo();

      ++k;
      cv.notify_one();
    }
  }

  void bar(function<void()> printBar) {
    unique_lock<mutex> guard(mtx);

    for (int i = 0; i < n; i++) {
      cv.wait(guard, [i, this]() { return this->k >= 2 * i + 1; });

      // printBar() outputs "bar". Do not change or remove this line.
      printBar();

      ++k;
      cv.notify_one();
    }
  }
};

// *****

int main() {
  return 0;
}
