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
  ListNode *reverseBetween(ListNode *head, int m, int n) {
    if (m == n)
      return head;
    ListNode pre_head(0, head);
    head = &pre_head;
    while (m > 1) {
      --m, --n;
      head = head->next;
    }
    ListNode *tail = head->next;
    ListNode *ntail = head->next->next;
    while (n > 1) {
      --n;
      ListNode *next_ntail = ntail->next;
      ntail->next = tail;
      tail = ntail;
      ntail = next_ntail;
    }
    head->next->next = ntail;
    head->next = tail;
    return pre_head.next;
  }
};

// *****

int main() {
  return 0;
}
