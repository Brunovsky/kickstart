#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;

// Segment tree solution

// *****

struct Range {
  u32 L, R;

  bool contains(Range r) const {
    return L <= r.L && R >= r.R;
  }

  friend bool operator<(Range lhs, Range rhs) {
    return lhs.L < rhs.L || (lhs.L == rhs.L && lhs.R < rhs.R);
  }
};

struct Book {
  Range range;
  u32 value = 0;
};

bool overlap(Range r1, Range r2) {
  return r1.R > r2.L && r2.R > r1.L;
}

Book *best_bookp(Book *lhs, Book *rhs) {
  if (lhs == rhs)
    return lhs;
  if (lhs == nullptr)
    return rhs;
  if (rhs == nullptr)
    return lhs;
  return lhs->value < rhs->value ? rhs : lhs;
}

Book *best_bookp(Book *cur, Book *lhs, Book *rhs) {
  if (lhs == nullptr && rhs == nullptr)
    return cur;
  return best_bookp(cur, best_bookp(lhs, rhs));
}

struct Node {
  Range range;
  Book *best = nullptr;
  u32 count = 0;
  set<Book *> books;
};

u32 N, Q, I, LR;
vector<Book> bookings; // bookings list
vector<Node> tree;     // ranges tree
vector<u32> endp;      // endpoints

// populate the tree with the ranges
void populate_tree(u32 i) {
  if (i >= I) {
    if (i >= LR) {
      tree[i].range = {endp[i - LR], endp[i - LR + 1]};
    } else {
      tree[i].range = {endp[I + i - LR], endp[I + i - LR + 1]};
    }
  } else {
    populate_tree(i << 1);
    populate_tree(i << 1 | 1);
    tree[i].range = {tree[i << 1].range.L, tree[i << 1 | 1].range.R};
  }
}

// insert book into the subtree below i, or on i.
void insert_book(Book *book, u32 i) {
  if (overlap(book->range, tree[i].range)) {
    if (book->range.contains(tree[i].range)) {
      tree[i].books.insert(book);
    } else {
      assert(i < I);
      insert_book(book, i << 1);
      insert_book(book, i << 1 | 1);
    }
  }
}

// fix book counts
void fix_counts(u32 i) {
  tree[i].count = tree[i >> 1].count + tree[i].books.size();
  if (i < I) {
    fix_counts(i << 1);
    fix_counts(i << 1 | 1);
  }
}

// found a leaf with just one book. Find the culprit book and propagate its
// best value up the tree while nullptrs are found.
void pushup(u32 i) {
  u32 j = i;
  while (j > 0 && tree[j].books.empty())
    j >>= 1;
  assert(j > 0 && tree[j].books.size() == 1);

  Book *book = *tree[j].books.begin();
  book->value += tree[i].range.R - tree[i].range.L;

  tree[j].best = book;

  while ((j >>= 1) > 0 && tree[j].best == nullptr) {
    tree[j].best = book;
  }
}

// decrement the book count on every leaf below i. Pushup new best books.
void rem_book_count(u32 i) {
  if (i < I) {
    rem_book_count(i << 1);
    rem_book_count(i << 1 | 1);
  } else {
    --tree[i].count;
    if (tree[i].count == 1)
      pushup(i);
  }
}

// decrement the book count on every leaf below i. Pushup new best books.
void search_open(u32 i) {
  if (i >= I) {
    if (tree[i].count == 1) {
      pushup(i);
    }
  } else {
    if (tree[i].count == 1) {
      // left
      if (tree[i << 1].books.empty()) {
        tree[i << 1].count = 1;
        search_open(i << 1);
      }
      // right
      if (tree[i << 1 | 1].books.empty()) {
        tree[i << 1 | 1].count = 1;
        search_open(i << 1 | 1);
      }
    } else if (tree[i].count == 0) {
      // left
      tree[i << 1].count = tree[i << 1].books.size();
      search_open(i << 1);
      // right
      tree[i << 1 | 1].count = tree[i << 1 | 1].books.size();
      search_open(i << 1 | 1);
    }
  }
}

// delete book from the subtree i.
void delete_book(Book *book, u32 i) {
  if (overlap(book->range, tree[i].range)) {
    if (tree[i].best == book)
      tree[i].best = nullptr;

    if (book->range.contains(tree[i].range)) {
      tree[i].books.erase(book);
      --tree[i].count;
      search_open(i);
      // rem_book_count(i);
    } else {
      assert(i < I);
      delete_book(book, i << 1);
      delete_book(book, i << 1 | 1);
    }

    if (i < I) {
      tree[i].best =
          best_bookp(tree[i].best, tree[i << 1].best, tree[i << 1 | 1].best);
    }
  }
}

// prepare the tree for the deletion cycle by pushing up all leaves already on
// count 1 and stabilizing the best fields on every node.
void stab_tree(u32 i) {
  if (i < I) {
    if (tree[i].best == nullptr)
      return;
    stab_tree(i << 1);
    stab_tree(i << 1 | 1);
    tree[i].best =
        best_bookp(tree[i].best, tree[i << 1].best, tree[i << 1 | 1].best);
  }
}

void prepare_tree() {
  for (auto &book : bookings)
    insert_book(&book, 1);
  fix_counts(1);
  for (u32 i = I; i < 2 * I; ++i) {
    if (tree[i].count == 1)
      pushup(i);
  }
  stab_tree(1);
}

void prepare_endp() {
  sort(endp.begin(), endp.end());
  endp.erase(unique(endp.begin(), endp.end()), endp.end());
  I = endp.size() - 1;
  tree.resize(2 * I);

  LR = I << 1;
  while ((LR & (LR - 1)) != 0)
    LR &= LR - 1;
}

u32 solve() {
  prepare_endp();
  populate_tree(1);
  prepare_tree();

  u32 best_value = UINT32_MAX, count = 0;

  while (tree[1].best != nullptr) {
    Book *best = tree[1].best;
    if (best->value < best_value)
      best_value = best->value;
    ++count;

    tree[1].best = nullptr;
    delete_book(best, 1);
  }

  return count == Q ? best_value : 0;
}

// *****

void reparse_test() {
  cin >> N >> Q;
  tree.clear();
  bookings.clear();
  endp.clear();
  for (u32 q = 0; q < Q; ++q) {
    u32 L, R;
    cin >> L >> R >> ws;
    ++R;
    endp.push_back(L);
    endp.push_back(R);
    bookings.push_back(Book{L, R, 0});
  }
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
