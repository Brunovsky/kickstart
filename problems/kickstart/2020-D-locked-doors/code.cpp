#include <bits/stdc++.h>

using namespace std;

// *****

#define RANGE(node) (node->R - node->L)

#define MAXN 100'000

int N;
int D[MAXN];

struct Node {
    Node* parent = nullptr;
    Node* left = nullptr;
    Node* right = nullptr;
    int L, R;
    vector<Node*> finger; // for queries

    Node(Node* parent, int L, int R) : parent(parent), L(L), R(R) {}
};

bool cmp_doors(int i, int j) {
    return D[i] > D[j];
}

bool cmp_nodes(Node* l, Node* r) {
    if (l->R < r->L) {
        return true;
    }
    if (r->R < l->L) {
        return false;
    }
    if (RANGE(l) != RANGE(r)) {
        return RANGE(l) < RANGE(r);
    }
    return l->L < r->L;
}

vector<Node*> ancestry;               // for building the tree
vector<unique_ptr<Node>> leaves;      // for queries
vector<unique_ptr<Node>> aggregators; // to free without recursion

Node* build_node(Node* parent, int L, int R) {
    auto node = make_unique<Node>(parent, L, R);
    if (L == R) {
        leaves[L] = move(node);
        return leaves[L].get();
    } else {
        aggregators.push_back(move(node));
        return aggregators.back().get();
    }
}

void build_finger_recursively(Node* node) {
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
    leaves.clear();
    aggregators.clear();
}

void prepare_tree() {
    vector<int> doors(N - 1);
    iota(begin(doors), end(doors), 0);
    sort(begin(doors), end(doors), cmp_doors);

    // I need to study more, fuck me this is a mess
    leaves.resize(N);

    set<Node*, decltype(*cmp_nodes)> built(cmp_nodes);
    Node* root = build_node(nullptr, 0, N - 1);
    built.insert(root);
    for (int door : doors) {
        Node dummy(nullptr, door, door);
        Node* parent = *built.upper_bound(&dummy);
        parent->left = build_node(parent, parent->L, door);
        parent->right = build_node(parent, door + 1, parent->R);
        built.insert(parent->left);
        built.insert(parent->right);
    }

    // build the finger tables
    build_finger_recursively(root);
}

int query(Node* node, int K) {
    if (RANGE(node) < K && RANGE(node->parent) >= K) {
        if (node->L != node->parent->L) {
            return node->R + 1 - K;
        } else {
            return node->L + 1 + K;
        }
    }
    // walk up the ancestry of this node until we find a parent whose range is >= K
    const auto& f = node->finger;
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
    memset(D, 0, sizeof(D));
    for (int i = 0; i + 1 < N; ++i) {
        cin >> D[i];
    }

    prepare_tree();

    for (int i = 0; i < Q; ++i) {
        int S, K;
        cin >> S >> K >> ws;

        if (K == 1) {
            cout << ' ' << S;
        } else {
            cout << ' ' << query(leaves[S - 1].get(), K - 1);
        }
    }

    free_nodes();
    return;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
        cout << '\n';
    }
    return 0;
}
