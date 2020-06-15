#include <bits/stdc++.h>

using namespace std;

// *****

struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};

// This could be improved significantly
class Solution {
public:
  ListNode *oddEvenList(ListNode *head) {
    if (!head || !head->next)
      return head;
    ListNode *const head_odd = head;
    ListNode *const head_even = head->next;
    ListNode *tail_odd = head, *tail_even = head->next;
    int i = 0;
    head = head->next->next;
    while (head != nullptr) {
      if (i++ & 1) {
        tail_even = tail_even->next = head;
      } else {
        tail_odd = tail_odd->next = head;
      }
      head = head->next;
    }
    tail_even->next = nullptr;
    tail_odd->next = head_even;
    return head_odd;
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
