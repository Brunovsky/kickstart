#include <bits/stdc++.h>

using namespace std;

// *****

#define MAX_SQUARE 10000000

vector<int> squares;

void populate_squares() {
    squares.reserve(3200);
    for (int n = 0; n * n <= MAX_SQUARE; ++n)
        squares.push_back(n * n);
}

inline int is_square(int n) {
    return (n >= 0 && binary_search(squares.begin(), squares.end(), n)) ? 1 : 0;
}

size_t count_squares_crossing(const vector<int> &A, int l, int m, int r) {
    assert(0 <= l && l < m && m < r);
    vector<int> sums_left;
    vector<int> sums_right;
    sums_left.reserve(m - l);
    sums_right.reserve(r - m);
    for (int i = m - 1, cur = 0; i >= l; --i) {
        cur += A[i];
        sums_left.push_back(cur);
    }
    for (int i = m, cur = 0; i < r; ++i) {
        cur += A[i];
        sums_right.push_back(cur);
    }
    sort(sums_left.begin(), sums_left.end());
    sort(sums_right.begin(), sums_right.end());

    int max_sum = sums_left.back() + sums_right.back();
    int max_L = sums_left.size(), max_R = sums_right.size();
    size_t mid_count = 0;
    for (int i = 0; squares[i] <= max_sum; ++i) {
        int sq = squares[i];
        int L = 0, R = max_R - 1;

        while (L < max_L && R >= 0) {
            int mid_sum = sums_left[L] + sums_right[R];
            if (mid_sum == sq) {
                int LL = L + 1, RR = R - 1;
                while (LL < max_L && sums_left[L] == sums_left[LL])
                    ++LL;
                while (RR >= 0 && sums_right[R] == sums_right[RR])
                    --RR;
                mid_count += size_t(LL - L) * size_t(R - RR);
                L = LL, R = RR;
            }
            if (mid_sum < sq)
                ++L;
            if (mid_sum > sq)
                --R;
        }
    }
    return mid_count;
}

size_t count_squares(const vector<int> &A, int l, int r) {
    assert(0 <= l && l < r);
    if (r - l < 4) {
        if (r - l == 1) {
            return is_square(A[l]);
        }
        if (r - l == 2) {
            return is_square(A[l]) + is_square(A[l + 1]) +
                   is_square(A[l] + A[l + 1]);
        }
        if (r - l == 3) {
            return is_square(A[l]) + is_square(A[l + 1]) + is_square(A[l + 2]) +
                   is_square(A[l] + A[l + 1]) + is_square(A[l + 1] + A[l + 2]) +
                   is_square(A[l] + A[l + 1] + A[l + 2]);
        }
    }
    int m = (l + r) / 2;
    size_t left_count = count_squares(A, l, m);
    size_t right_count = count_squares(A, m, r);
    size_t mid_count = count_squares_crossing(A, l, m, r);
    return left_count + right_count + mid_count;
}

auto solve() {
    int N;
    cin >> N >> ws;
    vector<int> A(N, 0);
    for (int i = 0; i < N; ++i)
        cin >> A[i];

    return count_squares(A, 0, N);
}

// *****

int main() {
    populate_squares();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
