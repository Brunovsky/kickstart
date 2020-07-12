#include <bits/stdc++.h>

using namespace std;

// *****

#define RANGE(node) (node->R - node->L)

int N;
vector<int> D;

struct Node {
    Node *parent = nullptr;
    Node *left = nullptr;
    Node *right = nullptr;
    int L, R;
    vector<Node *> finger;
};

vector<Node *> ancestry; // for building the tree
vector<Node *> leaves;
vector<Node *> aggregators;

Node *build_node(Node *parent, int L, int R) {
    Node *node = new Node();
    node->parent = parent;
    node->L = L;
    node->R = R;
    if (L == R) {
        leaves[L] = node;
    } else {
        aggregators.push_back(node);
    }
    return node;
}

void build_finger_recursively(Node *node) {
    if (!node)
        return;

    int pow2 = 1;
    int ancestor = int(ancestry.size()) - 1;
    while (ancestor >= 0) {
        node->finger.push_back(ancestry[ancestor]);
        ancestor -= pow2;
        pow2 *= 2;
    }

    ancestry.push_back(node);
    build_finger_recursively(node->left);
    build_finger_recursively(node->right);
    ancestry.pop_back();
}

void free_nodes() {
    for (Node *node : leaves)
        delete node;
    for (Node *node : aggregators)
        delete node;
}

void prepare_tree() {
    vector<int> doors(N - 1);
    iota(doors.begin(), doors.end(), 0);
    auto cmp_doors = [](int i, int j) { return D[i] > D[j]; };
    auto cmp_nodes = [](Node *l, Node *r) {
        if (l->R < r->L)
            return true;
        if (r->R < l->L)
            return false;
        if (RANGE(l) != RANGE(r))
            return RANGE(l) < RANGE(r);
        return l->L < r->L;
    };
    sort(doors.begin(), doors.end(), cmp_doors);

    // I need to study more, fuck me this is a mess
    ancestry.clear();
    leaves.resize(N);
    aggregators.clear();

    set<Node *, function<bool(Node *, Node *)>> built(cmp_nodes);
    Node *root = build_node(nullptr, 0, N - 1);
    built.insert(root);
    for (int door : doors) {
        Node dummy;
        dummy.L = door;
        dummy.R = door;
        Node *parent = *built.upper_bound(&dummy);
        parent->left = build_node(parent, parent->L, door);
        parent->right = build_node(parent, door + 1, parent->R);
        built.insert(parent->left);
        built.insert(parent->right);
    }

    // build the finger tables
    build_finger_recursively(root);
}

Node *query(Node *node, int K) {
    if (RANGE(node) < K && RANGE(node->parent) >= K) {
        return node;
    }
    const auto &f = node->finger;
    int i = 0;
    int F = f.size();
    do {
        ++i;
    } while (i < F && RANGE(f[i]) < K);
    return query(f[i - 1], K);
}

auto solve() {
    int Q;
    cin >> N >> Q >> ws;
    D.resize(N - 1);
    for (int i = 0; i + 1 < N; ++i)
        cin >> D[i];

    prepare_tree();

    vector<int> answers(Q);
    for (int i = 0; i < Q; ++i) {
        int S, K;
        cin >> S >> K >> ws;

        if (K == 1) {
            answers[i] = S;
            continue;
        }

        Node *node = query(leaves[S - 1], K - 1);
        if (node->L != node->parent->L) {
            answers[i] = node->R + 1 - (K - 1);
        } else {
            answers[i] = node->L + 1 + (K - 1);
        }
    }

    free_nodes();
    return answers;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ":";
        for (int ans : solution)
            cout << ' ' << ans;
        cout << '\n';
    }
    return 0;
}
