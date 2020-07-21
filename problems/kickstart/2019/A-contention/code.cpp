#include <bits/stdc++.h>

using namespace std;

// *****

#define OVERLAP(a, b) ((a).L <= (b).R && (b).L <= (a).R)
#define CONTAIN(a, b) ((a).L <= (b).L && (b).R <= (a).R)
#define LENGTH(a) ((a).R - (a).L + 1)

struct Range {
  int L, R;
};

struct Booking {
  int L, R;
  int seats;
  int id;
};

struct Node {
  int L, R;
  int leaf = 0;    // min of overlapping over all children of node
  int contain = 0; // number of bookings containing this range
  int overlap = 0; // number of bookings overlapping this range partially
  int delta = 0;
  set<int> books;
};

bool operator<(Range lhs, Range rhs) {
  return tie(lhs.L, lhs.R) < tie(rhs.L, rhs.R);
}

bool operator<(Booking lhs, Booking rhs) {
  return lhs.id < rhs.id;
}

bool cmp_seats(Booking lhs, Booking rhs) {
  if (lhs.seats != rhs.seats) {
    return lhs.seats < rhs.seats;
  } else {
    return lhs.id < rhs.id;
  }
}

using booking_cmp_t = function<bool(Booking, Booking)>;
using queue_t = priority_queue<Booking, vector<Booking>, booking_cmp_t>;

// *****

#define MAXQ 30001
#define MAXSZ 120001

int N, Q, O;
int endp[MAXSZ];
Node tree[MAXSZ];
Booking bookings[MAXQ];
queue_t bookings_queue(cmp_seats);

bool eliminated(int id) {
  return bookings[id].id == Q;
}

void eliminate(int id) {
  bookings[id].id = Q;
}

void pushdown(int i) {
  int delta = tree[i].delta;
  tree[i].delta = 0;
  for (int j : {i << 1, i << 1 | 1}) {
    tree[j].leaf += delta;
    tree[j].contain += delta;
    tree[j].overlap += delta;
    tree[j].delta += delta;
  }
}

void pushup(int i) {
  int lhs = tree[i << 1].leaf ? tree[i << 1].leaf : INT_MAX;
  int rhs = tree[i << 1 | 1].leaf ? tree[i << 1 | 1].leaf : INT_MAX;
  tree[i].leaf = min(min(lhs, rhs), tree[i].leaf);
}

// We found a node overlapped by only one interval, but the interval is large.
// The booking is stored in this node or one of its parents.
void walk_up(int i) {
  int root = i;
  while (root && tree[root].books.empty()) {
    root >>= 1;
  }
  assert(root && tree[root].books.size() == 1UL);
  int id = *tree[root].books.begin();
  assert(!eliminated(id));
  bookings[id].seats += LENGTH(tree[i]);
  bookings_queue.push(bookings[id]);
}

// Search for nodes with leaf=1, i.e. with open seats
// containing == 1 --> one interval around the current node
// overlapping == 1 --> one interval anywhere in current node's subtree
// therefore the booking is an ancestor of current node
void search_leaf(int i) {
  if (tree[i].overlap > 0 && tree[i].leaf <= 1) {
    if (tree[i].contain == 1 && tree[i].overlap == 1) {
      walk_up(i);
    } else {
      assert(i < O);
      pushdown(i);
      search_leaf(i << 1);
      search_leaf(i << 1 | 1);
      pushup(i);
    }
  }
}

void remove(int i, Booking book) {
  if (OVERLAP(book, tree[i])) {
    tree[i].overlap--;

    if (CONTAIN(book, tree[i])) {
      // we removed a 'contain' interval around this entire subtree
      tree[i].contain--;
      tree[i].leaf--;
      tree[i].delta--;
      tree[i].books.erase(book.id);
      search_leaf(i);
    } else {
      assert(i < O);
      pushdown(i);
      remove(i << 1, book);
      remove(i << 1 | 1, book);
      pushup(i);
    }
  }
}

void insert(int i, Booking book) {
  if (OVERLAP(book, tree[i])) {
    tree[i].overlap++;

    if (CONTAIN(book, tree[i])) {
      tree[i].contain++;
      tree[i].leaf++;
      tree[i].delta++;
      tree[i].books.insert(book.id);
    } else {
      assert(i < O);
      pushdown(i);
      insert(i << 1, book);
      insert(i << 1 | 1, book);
      pushup(i);
    }
  }
}

void clean() {
  N = Q = O = 0;
  for (int i = 0; i < MAXSZ; i++) {
    tree[i] = Node{};
  }
  memset(bookings, 0, sizeof(bookings));
  memset(endp, 0, sizeof(endp));
  // bookings_queue = queue_t(cmp_seats);
}

auto solve() {
  clean();
  cin >> N >> Q >> ws;

  for (int id = 0; id < Q; id++) {
    Booking book;
    cin >> book.L >> book.R >> ws;
    book.id = id;
    book.seats = 0;
    bookings[id] = book;
    endp[id << 1] = book.L;
    endp[id << 1 | 1] = book.R + 1;
  }
  sort(endp, endp + 2 * Q);
  O = unique(endp, endp + 2 * Q) - endp - 1;
  int P = 1;
  while (P < O) {
    P <<= 1;
  }

  for (int i = 0; i < O; i++) {
    tree[i + O].L = endp[i];
    tree[i + O].R = endp[i + 1] - 1;
  }
  rotate(tree + O, tree + (3 * O - P), tree + 2 * O);
  for (int i = O - 1; i > 0; i--) {
    tree[i].L = tree[i << 1].L;
    tree[i].R = tree[i << 1 | 1].R;
  }

  for (int id = 0; id < Q; id++) {
    insert(1, bookings[id]);
  }
  search_leaf(1);

  int reserved = 0;
  int k = N;
  while (!bookings_queue.empty()) {
    Booking copy = bookings_queue.top();
    bookings_queue.pop();

    int id = copy.id, seats = copy.seats;

    if (!eliminated(id)) {
      k = min(k, seats);
      reserved++;
      remove(1, bookings[id]);
      eliminate(id);
    }
  }

  return reserved == Q ? k : 0;
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
