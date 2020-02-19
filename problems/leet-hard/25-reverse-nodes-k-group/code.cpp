#include <bits/stdc++.h>

using namespace std;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// *****

struct ListNode {
  const int val;
  ListNode* next;
  ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
 public:
  ListNode* reverseKGroup(ListNode* head, int k) {
    if (head == nullptr || k <= 1) return head;

    ListNode *front = head->next, *tail = head;

    int c = 0;

    while (++c < k && front != nullptr) {
      ListNode* tmp = front->next;
      front->next = tail;
      tail = front;
      front = tmp;
    }

    if (c == k) {
      head->next = reverseKGroup(front, k);
      return tail;
    } else {
      head->next = nullptr;
      reverseKGroup(tail, c);
      return head;
    }
  }
};

// *****

void print(ListNode* head) {
  cout << "List: ";
  if (head != nullptr) {
    cout << head->val;
    head = head->next;
    while (head != nullptr) {
      cout << "->" << head->val;
      head = head->next;
    }
  }
  cout << '\n';
}

void test() {
  ListNode *list, *l;
  list = l = new ListNode(0);
  l = l->next = new ListNode(1);
  l = l->next = new ListNode(2);
  l = l->next = new ListNode(3);
  l = l->next = new ListNode(4);
  l = l->next = new ListNode(5);
  l = l->next = new ListNode(6);
  l = l->next = new ListNode(7);
  l = l->next = new ListNode(8);
  l = l->next = new ListNode(9);

  print(list);

  Solution S;
  ListNode* head = S.reverseKGroup(list, 4);

  print(head);
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  test();
  return 0;
}
