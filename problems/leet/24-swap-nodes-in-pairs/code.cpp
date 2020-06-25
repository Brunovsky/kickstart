#include <bits/stdc++.h>

using namespace std;

// *****

struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
  // leave last node unswapped if length is odd
  ListNode *swapPairs(ListNode *head) {
    auto tail = &head;
    while (*tail && (*tail)->next) {
      ListNode *b = *tail, *c = (*tail)->next;
      b->next = c->next;
      c->next = b;
      *tail = c;
      tail = &(b->next);
    }
    return head; // it got swapped with head->next
  }
};

// *****

int main() {
  return 0;
}
