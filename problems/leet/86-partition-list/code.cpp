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
  ListNode *partition(ListNode *head, int x) {
    ListNode lesser_prehead(0), greater_prehead(0);
    ListNode *lesser = &lesser_prehead;
    ListNode *greater = &greater_prehead;
    while (head) {
      if (head->val < x) {
        lesser = lesser->next = head;
      } else {
        greater = greater->next = head;
      }
      head = head->next;
    }
    greater->next = nullptr;
    lesser->next = greater_prehead.next;
    return lesser_prehead.next ? lesser_prehead.next : greater_prehead.next;
  }
};

// *****

int main() {
  return 0;
}
