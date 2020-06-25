#include <bits/stdc++.h>

using namespace std;

// *****

class ZeroEvenOdd {
private:
  int n;
  mutex mtx;
  condition_variable cv;
  int k = 1;

public:
  ZeroEvenOdd(int n) {
    this->n = n;
  }

  void zero(function<void(int)> printNumber) {
    unique_lock<mutex> guard(mtx);
    for (int i = 1; i <= n; ++i) {
      cv.wait(guard, [this]() { return k & 1; });
      printNumber(0);
      ++k;
      cv.notify_all();
    }
  }

  void even(function<void(int)> printNumber) {
    unique_lock<mutex> guard(mtx);
    for (int i = 2; i <= n; i += 2) {
      cv.wait(guard, [this]() { return !(k & 1) && !(k & 2); });
      printNumber(i);
      ++k;
      cv.notify_all();
    }
  }

  void odd(function<void(int)> printNumber) {
    unique_lock<mutex> guard(mtx);
    for (int i = 1; i <= n; i += 2) {
      cv.wait(guard, [this]() { return !(k & 1) && (k & 2); });
      printNumber(i);
      ++k;
      cv.notify_all();
    }
  }
};

// *****

int main() {
  return 0;
}
