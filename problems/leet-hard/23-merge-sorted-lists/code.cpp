#include <bits/stdc++.h>

using namespace std;

// *****

struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};

struct cmp {
  inline bool operator()(ListNode *const lhs, const ListNode *const rhs) const {
    return lhs->val > rhs->val;
  }
};

class Solution {
public:
  ListNode *mergeKLists(const vector<ListNode *> &lists) {
    priority_queue<ListNode *, deque<ListNode *>, cmp> lists_queue;
    ListNode *head;
    ListNode *tail;

    for (ListNode *headn : lists) {
      if (headn != nullptr)
        lists_queue.push(headn);
    }
    if (lists_queue.empty())
      return nullptr;

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

void print(ListNode *head) {
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
  ListNode *list1, *list2, *list3, *l;
  list1 = l = new ListNode(1);
  l = l->next = new ListNode(3);
  l = l->next = new ListNode(7);
  l = l->next = new ListNode(8);
  l = l->next = new ListNode(10);
  list2 = l = new ListNode(4);
  l = l->next = new ListNode(6);
  l = l->next = new ListNode(9);
  l = l->next = new ListNode(12);
  list3 = l = new ListNode(2);
  l = l->next = new ListNode(5);
  l = l->next = new ListNode(11);

  print(list1);
  print(list2);
  print(list3);

  Solution S;
  ListNode *head = S.mergeKLists({list1, list2, list3});

  print(head);
  cout << "The above should look like 1->2->...->11->12.\n";
}

// *****

int main() {
  test();
  return 0;
}
