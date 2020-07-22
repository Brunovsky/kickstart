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
  ListNode *mergeSortedLists(ListNode *a, ListNode *b) const {
    ListNode pre_head(0);
    ListNode *tail = &pre_head;
    while (a && b) {
      if (a->val < b->val) {
        tail = tail->next = a;
        a = a->next;
      } else {
        tail = tail->next = b;
        b = b->next;
      }
    }
    tail->next = a ? a : b;
    return pre_head.next;
  }

  ListNode *splitSort(ListNode *head, int len) const {
    int len_lhs = (len + 1) / 2;
    int len_rhs = len / 2;
    ListNode *split = head;
    for (int i = 1; i < len_lhs; ++i) {
      split = split->next;
    }
    ListNode *right = split->next;
    split->next = nullptr;
    head = sort(head, len_lhs);
    right = sort(right, len_rhs);
    return mergeSortedLists(head, right);
  }

  ListNode *sort(ListNode *head, int len) const {
    if (len == 2 && head->val > head->next->val) {
      ListNode *tail = head->next;
      tail->next = head;
      head->next = nullptr;
      head = tail;
    }
    if (len >= 3) {
      head = splitSort(head, len);
    }
    return head;
  }

public:
  ListNode *sortList(ListNode *head) const {
    ListNode *tail = head;
    int len = 0;
    while (tail) {
      tail = tail->next;
      ++len;
    }
    return sort(head, len);
  }
};

// *****

int main() {
  return 0;
}
