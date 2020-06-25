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
  ListNode *rotateRight(ListNode *head, int k) {
    if (!head)
      return nullptr;

    int len = 1;
    ListNode *tail = head;
    while (tail->next)
      ++len, tail = tail->next;

    k = k % len;
    if (k == 0)
      return head;

    ListNode *new_tail = head;
    while (++k < len)
      new_tail = new_tail->next;
    ListNode *new_head = new_tail->next;
    new_tail->next = nullptr;
    tail->next = head;
    return new_head;
  }
};

// *****

int main() {
  return 0;
}
