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
  ListNode *removeNthFromEnd(ListNode *head, int n) {
    ListNode *start = head, *end = head;
    while (n-- > 0)
      end = end->next;
    if (!end)
      return head->next; // release head
    while (end->next)
      end = end->next, start = start->next;
    start->next = start->next->next; // release start->next
    return head;
  }
};

// *****

int main() {
  return 0;
}
