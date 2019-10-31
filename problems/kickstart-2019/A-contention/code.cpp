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

Book* best_bookp(Book* lhs, Book* rhs) {
  if (lhs == rhs) return lhs;
  if (lhs == nullptr) return rhs;
  if (rhs == nullptr) return lhs;
  return lhs->value < rhs->value ? rhs : lhs;
}

Book* best_bookp(Book* cur, Book* lhs, Book* rhs) {
  if (lhs == nullptr && rhs == nullptr) return cur;
  return best_bookp(cur, best_bookp(lhs, rhs));
}

struct Node {
  Range range;
  Book* best = nullptr;
  u32 count = 0;
  set<Book*> books;
};

u32 N, Q, I, LR;
vector<Book> bookings;  // bookings list
vector<Node> tree;      // ranges tree
vector<u32> endp;       // endpoints

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

// increment the book count on every leaf below i.
void add_book_count(u32 i) {
  if (i < I) {
    add_book_count(i << 1);
    add_book_count(i << 1 | 1);
  } else {
    ++tree[i].count;
  }
}

// insert book into the subtree below i, or on i.
void insert_book(Book* book, u32 i) {
  if (overlap(book->range, tree[i].range)) {
    if (book->range.contains(tree[i].range)) {
      tree[i].books.insert(book);
      add_book_count(i);
    } else {
      assert(i < I);
      insert_book(book, i << 1);
      insert_book(book, i << 1 | 1);
    }
  }
}

// found a leaf with just one overlap. Find the culprit book and propagate its
// best value up the tree while nullptrs are found.
void pushup(u32 i) {
  u32 j = i;
  while (j > 0 && tree[j].books.empty()) j >>= 1;
  assert(j > 0 && tree[j].books.size() == 1);

  Book* book = *tree[j].books.begin();
  book->value += tree[i].range.R - tree[i].range.L;

  tree[j].best = book;

  while ((j >>= 1) > 0 && tree[j].best == nullptr) {
    tree[j].best = book;
  }
}

// decrement the book count on every leaf below i. Pushup new best contenders.
void rem_book_count(u32 i) {
  if (i < I) {
    rem_book_count(i << 1);
    rem_book_count(i << 1 | 1);
  } else {
    --tree[i].count;
    if (tree[i].count == 1) pushup(i);
  }
}

// delete book from the subtree i.
void delete_book(Book* book, u32 i) {
  if (overlap(book->range, tree[i].range)) {
    if (tree[i].best == book) tree[i].best = nullptr;

    if (book->range.contains(tree[i].range)) {
      tree[i].books.erase(book);
      rem_book_count(i);
    } else {
      assert(i < I);
      delete_book(book, i << 1);
      delete_book(book, i << 1 | 1);
    }

    if (i < I) {
      tree[i].best = best_bookp(tree[i].best, tree[i << 1].best, tree[i << 1 | 1].best);
    }
  }
}

// prepare the tree for the deletion cycle by pushing up all leaves already on
// count 1 and stabilizing the best fields on every node.
void stab_tree(u32 i) {
  if (i < I) {
    if (tree[i].best == nullptr) return;
    stab_tree(i << 1);
    stab_tree(i << 1 | 1);
    tree[i].best = best_bookp(tree[i].best, tree[i << 1].best, tree[i << 1 | 1].best);
  }
}

void prepare_tree() {
  for (auto& book : bookings) insert_book(&book, 1);
  for (u32 i = I; i < 2 * I; ++i) {
    if (tree[i].count == 1) pushup(i);
  }
  stab_tree(1);
}

void prepare_endp() {
  sort(endp.begin(), endp.end());
  endp.erase(unique(endp.begin(), endp.end()), endp.end());
  I = endp.size() - 1;
  tree.resize(2 * I);

  LR = I << 1;
  while ((LR & (LR - 1)) != 0) LR &= LR - 1;
}

u32 solve() {
  prepare_endp();
  populate_tree(1);
  prepare_tree();

  u32 best_value = UINT32_MAX, count = 0;

  while (tree[1].best != nullptr) {
    Book* best = tree[1].best;
    if (best->value < best_value) best_value = best->value;
    ++count;

    tree[1].best = nullptr;
    delete_book(best, 1);
  }

  return count == Q ? best_value : 0;
}

// *****

void prep() {}

void parse_test() {
  cin >> N >> Q;
  for (u32 q = 0; q < Q; ++q) {
    u32 L, R;
    cin >> L >> R >> ws;
    ++R;
    endp.push_back(L);
    endp.push_back(R);
    bookings.push_back(Book {L, R, 0});
  }
}

void reset_test() {
  tree.clear();
  bookings.clear();
  endp.clear();
}

void print_test(u64 casenum, u32 solution) {
  cout << "Case #" << casenum << ": ";
  cout << solution;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  prep();
  u64 tree;
  cin >> tree >> ws;
  for (u64 t = 0; t < tree; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
