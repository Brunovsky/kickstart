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
  ListNode *mergeTwoLists(ListNode *l1, ListNode *l2) {
    if (!l1 || !l2)
      return l1 ? l1 : l2;
    if (l1->val > l2->val)
      swap(l1, l2);
    ListNode *result = l1, *start = result;
    l1 = l1->next;
    while (l1 && l2) {
      ListNode *&min = l1->val <= l2->val ? l1 : l2;
      result = result->next = min;
      min = min->next;
    }
    result->next = l1 ? l1 : l2;
    return start;
  }
};

// *****

int main() {
  return 0;
}
