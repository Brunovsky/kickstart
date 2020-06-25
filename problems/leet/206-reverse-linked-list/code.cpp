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
  ListNode *reverseList(ListNode *head) {
    if (!head)
      return nullptr;
    ListNode *left = head, *right = head->next, *tail;
    head->next = nullptr;
    while (right) {
      tail = right->next;
      right->next = left;
      left = right;
      right = tail;
    }
    return left;
  }
};

// *****

int main() {
  return 0;
}
