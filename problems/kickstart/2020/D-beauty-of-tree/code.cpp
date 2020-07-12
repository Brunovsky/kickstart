#include <bits/stdc++.h>

using namespace std;

// *****

int N, A, B;
vector<int> parent;
vector<vector<int>> children;

size_t painted_A, painted_B, painted_AB;
vector<size_t> count_A, count_B;

void visit(int node, int depth) {
    int ai = depth % A;
    int bi = depth % B;
    int saved_a = count_A[ai];
    int saved_b = count_B[bi];

    for (int child : children[node])
        visit(child, depth + 1);

    ++count_A[ai], ++count_B[bi];
    size_t node_A = count_A[ai] - saved_a;
    size_t node_B = count_B[bi] - saved_b;

    painted_A += node_A;
    painted_B += node_B;
    painted_AB += node_A * node_B;
}

auto solve() {
    cin >> N >> A >> B >> ws;
    parent.resize(N + 1);
    children.assign(N + 1, {});
    parent[1] = 1;

    for (int i = 2; i <= N; ++i) {
        cin >> parent[i];
        children[parent[i]].push_back(i);
    }

    painted_A = painted_B = painted_AB = 0;
    count_A.assign(A, {});
    count_B.assign(B, {});
    visit(1, 0);

    double expected_A = double(painted_A) / N;
    double expected_B = double(painted_B) / N;
    double expected_AB = double(painted_AB) / (double(N) * double(N));
    return expected_A + expected_B - expected_AB;
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(9);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
