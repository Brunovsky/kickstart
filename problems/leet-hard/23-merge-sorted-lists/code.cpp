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
  int val;
  ListNode* next;
  ListNode(int x) : val(x), next(NULL) {}
};

struct cmp {
  inline bool operator()(ListNode* const lhs, const ListNode* const rhs) const {
    return lhs->val > rhs->val;
  }
};

class Solution {
 public:
  ListNode* mergeKLists(const vector<ListNode*>& lists) {
    priority_queue<ListNode*, deque<ListNode*>, cmp> lists_queue;
    ListNode* head;
    ListNode* tail;

    for (ListNode* headn : lists) {
      if (headn != nullptr) lists_queue.push(headn);
    }
    if (lists_queue.empty()) return nullptr;

    head = tail = lists_queue.top();
    lists_queue.pop();

    while (!lists_queue.empty()) {
      if (tail->next != nullptr) {
        lists_queue.push(tail->next);
      }
      tail->next = lists_queue.top();
      lists_queue.pop();
      tail = tail->next;
    }

    return head;
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
  list = l = new ListNode(1);
  l = l->next = new ListNode(2);
  l = l->next = new ListNode(3);
  l = l->next = new ListNode(4);
  l = l->next = new ListNode(5);

  print(list);

  Solution S;
  ListNode* head = S.mergeKLists(list);

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
