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
  ListNode *deleteDuplicates(ListNode *head) {
    ListNode **curr = &head;
    while (*curr && (*curr)->next) {
      int val = (*curr)->val;
      if (val == (*curr)->next->val) {
        ListNode *tail = (*curr)->next->next;
        while (tail && tail->val == val)
          tail = tail->next;
        *curr = tail;
      } else {
        curr = &(*curr)->next;
      }
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
