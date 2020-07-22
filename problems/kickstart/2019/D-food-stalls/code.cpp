#include <bits/stdc++.h>

using namespace std;
using i64 = int64_t;

// *****

struct Post {
  i64 C, D;
};

inline bool operator==(const Post &lhs, const Post &rhs) {
  return lhs.D == rhs.D;
}

inline bool operator<(const Post &lhs, const Post &rhs) {
  return lhs.D < rhs.D;
}

inline bool less_left(Post lhs, Post rhs) {
  i64 diff = (lhs.C - lhs.D) - (rhs.C - rhs.D);
  return diff < 0 || (diff == 0 && lhs.D < rhs.D);
}

inline bool less_right(Post lhs, Post rhs) {
  i64 diff = (lhs.C + lhs.D) - (rhs.C + rhs.D);
  return diff < 0 || (diff == 0 && lhs.D < rhs.D);
}

inline bool greater_left(Post lhs, Post rhs) {
  i64 diff = (lhs.C - lhs.D) - (rhs.C - rhs.D);
  return diff > 0 || (diff == 0 && lhs.D < rhs.D);
}

inline bool greater_right(Post lhs, Post rhs) {
  i64 diff = (lhs.C + lhs.D) - (rhs.C + rhs.D);
  return diff > 0 || (diff == 0 && lhs.D < rhs.D);
}

inline i64 cost_left(Post l, int wD) {
  return (wD - l.D) + l.C;
}

inline i64 cost_right(Post r, int wD) {
  return (r.D - wD) + r.C;
}

int K, N;

vector<Post> posts;

auto solve() {
  cin >> K >> N >> ws;
  posts.assign(N, {});
  for (int i = 0; i < N; ++i)
    cin >> posts[i].D;
  for (int i = 0; i < N; ++i)
    cin >> posts[i].C;

  i64 minimum = 0;
  sort(posts.begin(), posts.end());

  set<Post, bool (*)(Post, Post)> pending_left(less_left);
  set<Post, bool (*)(Post, Post)> pending_right(less_right);
  set<Post, bool (*)(Post, Post)> working_left(greater_left);
  set<Post, bool (*)(Post, Post)> working_right(greater_right);
  for (int i = 1; i < N; ++i)
    pending_right.insert(posts[i]);

  i64 sum_left = 0, sum_right = 0;

  // Warehouse at post[0]
  {
    Post warehouse = posts[0];
    int count = 0;
    for (auto it = pending_right.begin(); count < K; ++count) {
      Post post = *it;
      it = pending_right.erase(it);
      working_right.insert(post);
      sum_right += post.C + post.D;
    }
    minimum = sum_left + sum_right - K * warehouse.D + warehouse.C;
  }

  // Warehouse at post[1..N]
  for (int w = 1; w < N; ++w) {
    Post warehouse = posts[w];

    // insert post[w-1]
    i64 cost_new_left = cost_left(posts[w - 1], warehouse.D);
    pending_left.insert(posts[w - 1]);

    // remove post[w], replace it with best pending stall @left
    if (working_right.count(warehouse)) {
      working_right.erase(warehouse);
      sum_right -= warehouse.C + warehouse.D;

      Post left = *pending_left.begin();
      working_left.insert(left);
      sum_left += left.C - left.D;

      pending_left.erase(pending_left.begin());
    } else {
      pending_right.erase(warehouse);

      // worst working stall @right => new left post[w-1]
      if (!working_right.empty()) {
        Post right = *working_right.begin();

        i64 worst_w_right = cost_right(right, warehouse.D);

        if (cost_new_left < worst_w_right) {
          pending_right.insert(right);
          working_right.erase(working_right.begin());
          sum_right -= right.C + right.D;

          pending_left.erase(posts[w - 1]);
          working_left.insert(posts[w - 1]);
          sum_left += posts[w - 1].C - posts[w - 1].D;
        }
      }
      // worst working stall @left => new left post[w-1]
      else if (!working_left.empty()) {
        Post left = *working_left.begin();

        i64 worst_w_left = cost_left(left, warehouse.D);

        if (cost_new_left < worst_w_left) {
          pending_left.insert(left);
          working_left.erase(working_left.begin());
          sum_left -= left.C - left.D;

          pending_left.erase(posts[w - 1]);
          working_left.insert(posts[w - 1]);
          sum_left += posts[w - 1].C - posts[w - 1].D;
        }
      }
    }

    // worst working stall @left => best pending stall @right
    while (!working_left.empty() && !pending_right.empty()) {
      Post left = *working_left.begin();
      Post right = *pending_right.begin();

      i64 worst_w_left = cost_left(left, warehouse.D);
      i64 best_p_right = cost_right(right, warehouse.D);

      if (best_p_right < worst_w_left) {
        working_right.insert(right);
        pending_right.erase(pending_right.begin());
        sum_right += right.C + right.D;

        pending_left.insert(left);
        working_left.erase(working_left.begin());
        sum_left -= left.C - left.D;
      } else {
        break;
      }
    }

    i64 shift = i64(working_left.size()) - i64(working_right.size());
    i64 cost = sum_left + sum_right + shift * warehouse.D + warehouse.C;
    if (minimum > cost)
      minimum = cost;

    // cout << "=== WAREHOUSE " << w << " | cost: " << cost << endl;
    // for (Post post : pending_left) cout << '[' << post.C << ' ' << post.D <<
    // "] "; cout << endl; for (Post post : working_left) cout << '[' << post.C
    // << ' ' << post.D << "] "; cout << endl; for (Post post : pending_right)
    // cout << '[' << post.C << ' ' << post.D << "] "; cout << endl; for (Post
    // post : working_right) cout << '[' << post.C << ' ' << post.D << "] ";
    // cout << endl << "=== === === === ===\n";
  }

  return minimum;
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
