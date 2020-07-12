#include <bits/stdc++.h>

using namespace std;

// *****

int N, A, B;
vector<vector<int>> children;

size_t sum_A, sum_B, sum_AB;
vector<size_t> count_A, count_B;

// visit in postorder
void visit(int node, int depth) {
    int ai = depth % A;
    int bi = depth % B;
    int pre_A = count_A[ai];
    int pre_B = count_B[bi];

    for (int child : children[node])
        visit(child, depth + 1);

    ++count_A[ai], ++count_B[bi];
    size_t node_A = count_A[ai] - pre_A;
    size_t node_B = count_B[bi] - pre_B;

    sum_A += node_A;
    sum_B += node_B;
    sum_AB += node_A * node_B;
}

auto solve() {
    cin >> N >> A >> B >> ws;
    children.assign(N + 1, {});

    for (int i = 2; i <= N; ++i) {
        int parent;
        cin >> parent;
        children[parent].push_back(i);
    }

    sum_A = sum_B = sum_AB = 0;
    count_A.assign(A, {});
    count_B.assign(B, {});
    visit(1, 0);

    double expected_A = double(sum_A) / N;
    double expected_B = double(sum_B) / N;
    double expected_AB = double(sum_AB) / (double(N) * double(N));
    return expected_A + expected_B - expected_AB;
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(7);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
