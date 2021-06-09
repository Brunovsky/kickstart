#include "test_utils.hpp"
#include "../struct/crtp/splay_tree.hpp"

inline namespace unit_testing_splay_tree {

struct splay_data : splay_order<splay_data> {
    char name = '0';
    int self = 0;
    int subt = 0;
    int lazy = 0;

    splay_data(char name, int self) : name(name), self(self) {}
    static int get_subt(const splay_data* n) { return n ? n->subt : 0; }

    void pushdown() {
        splay_order<splay_data>::pushdown();
        if (lazy) {
            if (child[0]) child[0] += lazy;
            if (child[1]) child[1] += lazy;
            self += lazy;
            subt += lazy * size();
            lazy = 0;
        }
    }

    void pushup() {
        splay_order<splay_data>::pushup();
        subt = self + get_subt(child[0]) + get_subt(child[1]);
    }
};

using Splay = splay_data;

void print_dfs(Splay* root, int depth = 0, char label = ' ') {
    if (root) {
        print("{} {} {} (size={}, self={}, subt={})\n", string(4 * depth, ' '), label,
              root->name, root->size(), root->self, root->subt);
        print_dfs(root->child[0], depth + 1, '-');
        print_dfs(root->child[1], depth + 1, '+');
    }
}

void unit_test_splay_tree() {
    int N = 8;
    Splay* A = new Splay('A', 30);
    Splay* B = new Splay('B', 20);
    Splay* C = new Splay('C', 70);
    Splay* D = new Splay('D', 10);
    Splay* E = new Splay('E', 20);
    Splay* F = new Splay('F', 40);
    Splay* G = new Splay('G', 50);
    Splay* H = new Splay('H', 60);
    // C F A B G E D H, starting with A
    A->insert_after(B);
    A->insert_before(C);
    B->insert_after(D);
    D->insert_before(E);
    C->insert_after(F);
    B->insert_after(G);
    D->insert_after(H);
    A->splay();
    print_dfs(A);
    D->splay();
    print_dfs(D);
    for (Splay* node : {A, B, C, D, E, F, G, H}) {
        print("Order of {}: {}\n", node->name, node->order_of_node());
    }
    for (int i = 0; i < N; i++) {
        print("Node of order {}: {}\n", i, D->find_by_order(i)->name);
    }
    G->splay();
    print_dfs(G);
    Splay* L = G->detach_left();
    print_dfs(L);
    print_dfs(G);
    print_dfs(Splay::join(G, L));
    B->splay();
    C->splay();
    A->splay();
    print_dfs(A);
    F->splay();
    E->splay();
    print_dfs(F);
    H->splay();
    print_dfs(H);
}

} // namespace unit_testing_splay_tree

int main() {
    RUN_SHORT(unit_test_splay_tree());
    return 0;
}
