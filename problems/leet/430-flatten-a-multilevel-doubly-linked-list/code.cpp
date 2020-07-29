#include <bits/stdc++.h>

using namespace std;

// *****

struct Node {
  int val;
  Node *prev;
  Node *next;
  Node *child;
};

class Solution {
  void flat(Node *curr, Node *tail) const {
    assert(curr != nullptr);
    while (curr->next) {
      Node *next = curr->next;
      if (curr->child) {
        flat(curr->child, next);
        curr->child->prev = curr;
        curr->next = curr->child;
        curr->child = nullptr;
      }
      curr = next;
    }
    if (curr->child) {
      flat(curr->child, tail);
      curr->child->prev = curr;
      curr->next = curr->child;
      curr->child = nullptr;
    } else if (tail != nullptr) {
      curr->next = tail;
      tail->prev = curr;
    }
  }

public:
  Node *flatten(Node *head) {
    if (head == nullptr)
      return head;
    flat(head, nullptr);
    return head;
  }
};

// *****

int main() {
  return 0;
}
