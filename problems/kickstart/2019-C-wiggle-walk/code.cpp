#include <bits/stdc++.h>

using namespace std;

// *****

struct pos_t {
    int r, c;
};

bool operator==(pos_t lhs, pos_t rhs) {
    return lhs.r == rhs.r && lhs.c == rhs.c;
}

struct hasher {
    static constexpr size_t mask = (CHAR_BIT * sizeof(size_t)) >> 1;
    inline size_t operator()(pos_t pos) const {
        size_t r = hash<int>{}(pos.r);
        size_t c = hash<int>{}(pos.c);
        return (r + 0x379f177f) ^ ((c << mask) | (c >> mask));
    }
};

struct Node {
    int r = 0, c = 0;
    int N = 0, S = 0, W = 0, E = 0;
};

int I, R, C, SR, SC;
string commands;
unordered_map<pos_t, Node, hasher> nodes;

void insert(pos_t pos) {
    int r = pos.r, c = pos.c;
    Node node{r, c, r - 1, r + 1, c - 1, c + 1};
    nodes[pos] = node;
}

void remove(Node node) {
    pos_t N = {node.N, node.c};
    pos_t S = {node.S, node.c};
    pos_t W = {node.r, node.W};
    pos_t E = {node.r, node.E};
    if (!nodes.count(N)) {
        insert(N);
    }
    if (!nodes.count(S)) {
        insert(S);
    }
    if (!nodes.count(W)) {
        insert(W);
    }
    if (!nodes.count(E)) {
        insert(E);
    }
    nodes[N].S = node.S;
    nodes[S].N = node.N;
    nodes[W].E = node.E;
    nodes[E].W = node.W;
}

pair<int, int> solve() {
    cin >> I >> R >> C >> SR >> SC >> ws;
    cin >> commands >> ws;
    assert(commands.size() == size_t(I));

    nodes.clear();

    int r = SR, c = SC;
    insert({r, c});

    for (char command : commands) {
        remove(nodes[{r, c}]);
        if (command == 'N') {
            r = nodes[{r, c}].N;
        }
        if (command == 'W') {
            c = nodes[{r, c}].W;
        }
        if (command == 'S') {
            r = nodes[{r, c}].S;
        }
        if (command == 'E') {
            c = nodes[{r, c}].E;
        }
    }

    return {r, c};
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution.first << ' ' << solution.second << '\n';
    }
    return 0;
}
