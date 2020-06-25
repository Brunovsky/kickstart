#include <bits/stdc++.h>

using namespace std;

// *****

class Foo {
  mutex waiting;
  condition_variable cv;
  int index = 1;

public:
  Foo() {}

  void first(function<void()> printFirst) {
    unique_lock<mutex> lock(waiting);

    // obviously this is dead code
    if (index < 1)
      cv.wait(lock, [&]() { return index == 1; });

    // printFirst() outputs "first". Do not change or remove this line.
    printFirst();

    ++index;
    cv.notify_all();
  }

  void second(function<void()> printSecond) {
    unique_lock<mutex> lock(waiting);
    if (index < 2)
      cv.wait(lock, [&]() { return index == 2; });

    // printSecond() outputs "second". Do not change or remove this line.
    printSecond();

    ++index;
    cv.notify_all();
  }

  void third(function<void()> printThird) {
    unique_lock<mutex> lock(waiting);
    if (index < 3)
      cv.wait(lock, [&]() { return index == 3; });

    // printThird() outputs "third". Do not change or remove this line.
    printThird();

    ++index;
    cv.notify_all();
  }
};

// *****

int main() {
  return 0;
}
