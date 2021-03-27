#include <bits/stdc++.h>

using namespace std;

// *****

default_random_engine mt(random_device{}());
using intd = uniform_int_distribution<int>;
using pt = array<int, 2>;

pt different(int lo, int hi) {
    intd any(lo, hi), diff(lo, hi - 1);
    int u = any(mt), v = diff(mt);
    return u <= v ? pt{u, v + 1} : pt{v, u};
}

int N, Q;
int A[51]; // 1 to N inclusive

int QUERY(int a, int b, int c) {
    string s = to_string(A[a]) + ' ' + to_string(A[b]) + ' ' + to_string(A[c]) + '\n';
    cout << s;
    int n;
    cin >> n;
    if (n != A[a] && n != A[b] && n != A[c]) {
        exit(0);
    }
    return n == A[a] ? a : n == A[b] ? b : c;
}

void ANSWER() {
    string s;
    for (int i = 1; i <= N; i++) {
        s += to_string(A[i]) + " ";
    }
    s.back() = '\n';
    cout << s;
    int n;
    cin >> n;
    if (n != 1) {
        exit(0);
    }
}

auto pivot(int L, int R) {
    auto [l, r] = different(L, R);
    if (L > 1 && QUERY(L - 1, l, r) == r)
        swap(l, r);
    else if (R < N && QUERY(l, r, R + 1) == l)
        swap(l, r);
    return pt{l, r};
}

void recurse_sort(int L, int R) {
    int n = R - L + 1;
    if (n >= 3) {
        auto [l, r] = pivot(L, R);
        vector<int> left, middle, right;
        for (int i = L; i <= R; i++) {
            if (i != l && i != r) {
                int median = QUERY(i, l, r);
                if (median == l)
                    left.push_back(A[i]);
                else if (median == i)
                    middle.push_back(A[i]);
                else
                    right.push_back(A[i]);
            }
        }

        int i = L, a = A[l], b = A[r];
        for (int x : left)
            A[i++] = x;
        l = i;
        A[i++] = a;
        for (int x : middle)
            A[i++] = x;
        r = i;
        A[i++] = b;
        for (int x : right)
            A[i++] = x;

        recurse_sort(L, l - 1);
        recurse_sort(l + 1, r - 1);
        recurse_sort(r + 1, R);
    }
    if (n == 2) {
        if (L > 1 && QUERY(L - 1, L, R) == R)
            swap(A[L], A[R]);
        else if (R < N && QUERY(L, R, R + 1) == L)
            swap(A[L], A[R]);
    }
}

void solve() {
    iota(A, A + N + 1, 0);
    recurse_sort(1, N);
    ANSWER();
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    unsigned T;
    cin >> T >> N >> Q;
    Q /= T;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
