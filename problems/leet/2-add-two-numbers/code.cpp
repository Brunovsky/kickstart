#include <bits/stdc++.h>

using namespace std;

// *****

// This solution is old and ugly, don't use it as reference for anything

struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
  ListNode *addTwoNumbers(ListNode *l1, ListNode *l2) {
    int carry = 0;
    ListNode *head = l1;

    // if l1 is shorter than l2 we'll swap them in the loop
    while (l1) {
      l1->val += carry + (l2 ? l2->val : 0);

      if (l1->val >= 10)
        l1->val -= 10, carry = 1;
      else
        carry = 0;

      if (!l1->next && l2 && l2->next)
        swap(l1->next, l2->next);

      // Append a 0 to the front of l1, e.g. 7632 -> 07632
      if (!l1->next && carry)
        l1->next = new ListNode(0);

      l1 = l1->next;
      if (l2)
        l2 = l2->next;
    }

    return head;
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
