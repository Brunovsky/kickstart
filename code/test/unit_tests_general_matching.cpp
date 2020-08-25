#include "../general_matching.hpp"

// *****

constexpr int A = 10, B = 11, C = 12, D = 13, E = 14;
constexpr int F = 15, G = 16, H = 17, I = 18, J = 19;
constexpr int K = 20, L = 21, M = 22, N = 23;

void debug_header(string name) {
    dflash();
    print("{}{}{}\n", string(70, '~'), name, string(70, '~'));
    dflash();
}

/**
 *
 * [0]-----[1]~ ~ ~[2]-----[3]~ ~ ~[4]-----[5]~ ~ ~[6]-----[7]
 *
 *  0       1       2       3       3       2       1       0
 *
 * The augmenting path to be found is 0->1->2->3->4->5->6->7.
 * The bridge is 3-4.
 */
void test_straight_line_1() {
    debug_header("test straight line 1");
    micali_vazirani vg(8);
    vg.add(0, 1); // 0
    vg.add(1, 2); // 1 ~
    vg.add(2, 3); // 2
    vg.add(3, 4); // 3 ~
    vg.add(4, 5); // 4
    vg.add(5, 6); // 5 ~
    vg.add(6, 7); // 6
    vg.mate[1] = vg.mate[2] = 1;
    vg.mate[3] = vg.mate[4] = 3;
    vg.mate[5] = vg.mate[6] = 5;
    int augmentation = vg.max_matching(true);
    cout << "AUG: " << augmentation << endl;
    assert(augmentation == 4);
}

/**
 *
 * [0]-----[1]~ ~ ~[2]-----[3]
 *  |                       |
 *  |                       |
 * [4]~ ~ ~[5]-----[6]~ ~ ~[7]
 *
 * The augmenting path to be found is 0->1->2->3.
 * The bridge is 1-2.
 */
void test_straight_line_2() {
    debug_header("test straight line 2");
    micali_vazirani vg(8);
    vg.add(0, 1); // 0
    vg.add(1, 2); // 1 ~
    vg.add(2, 3); // 2
    vg.add(0, 4); // 3
    vg.add(4, 5); // 4 ~
    vg.add(5, 6); // 5
    vg.add(6, 7); // 6 ~
    vg.add(3, 7); // 7
    vg.mate[1] = vg.mate[2] = 1;
    vg.mate[4] = vg.mate[5] = 4;
    vg.mate[6] = vg.mate[7] = 6;
    int augmentation = vg.max_matching(true);
    cout << "AUG: " << augmentation << endl;
    assert(augmentation == 4);
}

/**
 *                 [3]             [7]
 *                 / ~             ~
 *                /   ~           ~
 *               /     ~         ~
 *     [1]~ ~ ~[2]     [4]     [6]
 *       \     /         \     /
 *        \   /           \   /
 *         \ /             \ /
 *         [0]             [5]
 *
 * The augmenting path to be found is 0->1->2->3->4->5.
 * The bridge is 2-3.
 */
void test_immediate_blossom_1() {
    debug_header("immediate blossom 1");
    micali_vazirani vg(8);
    vg.add(0, 1); // 0
    vg.add(0, 2); // 1
    vg.add(1, 2); // 2 ~
    vg.add(2, 3); // 3
    vg.add(3, 4); // 4 ~
    vg.add(4, 5); // 5
    vg.add(5, 6); // 6
    vg.add(6, 7); // 7 ~
    vg.mate[1] = vg.mate[2] = 2;
    vg.mate[3] = vg.mate[4] = 4;
    vg.mate[6] = vg.mate[7] = 7;
    int augmentation = vg.max_matching(true);
    cout << "AUG: " << augmentation << endl;
    assert(augmentation == 4);
}

/**
 *
 *     [1]~ ~ ~[2]     [3]~ ~ ~[4]
 *       \       \     /       / \
 *        \_______\   /_______/   \
 *                \\ //            \
 *                 [0]             [5]
 *
 * The augmenting path to be found is 0->3->4->5.
 * The bridge is 3-4.
 */
void test_immediate_blossom_2() {
    debug_header("immediate blossom 2");
    micali_vazirani vg(6);
    vg.add(0, 1); // 0
    vg.add(0, 2); // 1
    vg.add(0, 3); // 2
    vg.add(0, 4); // 3
    vg.add(1, 2); // 4 ~
    vg.add(3, 4); // 5 ~
    vg.add(4, 5); // 6
    vg.mate[1] = vg.mate[2] = 4;
    vg.mate[3] = vg.mate[4] = 5;
    int augmentation = vg.max_matching(true);
    cout << "AUG: " << augmentation << endl;
    assert(augmentation == 3);
}

/**
 *
 *     [1]~ ~ ~[2]
 *       \     / \
 *        \   /   \
 *         \ /     \
 *         [0]     [3]
 *
 * The augmenting path to be found is 0->1->2->3.
 */
void test_immediate_blossom_3() {
    debug_header("immediate blossom 3");
    micali_vazirani vg(4);
    vg.add(0, 1); // 0
    vg.add(0, 2); // 1
    vg.add(1, 2); // 2 ~
    vg.add(2, 3); // 3
    vg.mate[1] = vg.mate[2] = 2;
    int augmentation = vg.max_matching(true);
    cout << "AUG: " << augmentation << endl;
    assert(augmentation == 2);
}

/**
 *
 *           [5]~ ~ ~[6]-----[7]~ ~ ~[8]
 *           / \     /         \     /
 *          /   \   /           \   /
 *         /     \ /             \ /
 *       [B]     [3]-------------[4]
 *        ~        ~             ~
 *        ~         ~           ~
 *        ~          ~         ~
 *       [A]         [1]     [2]
 *        |            \     /
 *        |             \   /
 *        |              \ /
 *       [9]             [0]
 *
 * The augmenting path to be found is 0->1->3->6->5->B->A->9.
 */
void test_pyramid_blossom_1() {
    debug_header("pyramid blossom 1");
    micali_vazirani vg(12);
    vg.add(0, 1); // 0
    vg.add(0, 2); // 1
    vg.add(1, 3); // 2 ~
    vg.add(2, 4); // 3 ~
    vg.add(3, 5); // 4
    vg.add(3, 6); // 5
    vg.add(4, 7); // 6
    vg.add(4, 8); // 7
    vg.add(3, 4); // 8
    vg.add(5, 6); // 9 ~
    vg.add(6, 7); // 10
    vg.add(7, 8); // 11 ~
    vg.add(9, A); // 12
    vg.add(A, B); // 13 ~
    vg.add(B, 5); // 14
    vg.mate[1] = vg.mate[3] = 2;
    vg.mate[2] = vg.mate[4] = 3;
    vg.mate[5] = vg.mate[6] = 9;
    vg.mate[7] = vg.mate[8] = 11;
    vg.mate[A] = vg.mate[B] = 13;
    int cardinality = vg.max_matching(true);
    assert(cardinality == 6);
}

/**
 *
 *  [7]~ ~ ~ ~ ~ ~ ~ ~ ~[8]-----[9]
 *   |                   |       ~
 *   |                   |       ~
 *   |                   |      [A]
 *  [3]~ ~ ~[4] [5]~ ~ ~[6]      |
 *    \     /     \     /        |
 *     \   /       \   /        [B]
 *      \ /         \ /          ~
 *      [1]~ ~ ~ ~ ~[2]          ~
 *        \_       _/           [C]
 *          \_   _/              |
 *            \ /                |
 *            [0]               [D]
 *
 * There is only one augmentation path to be found: 0->2->1->4->3->7->8->9->A->B->C->D.
 * The bridge is 7~8.
 */
void test_pyramid_blossom_2() {
    debug_header("pyramid blossom 2");
    micali_vazirani vg(14);
    vg.add(0, 1); // 0
    vg.add(0, 2); // 1
    vg.add(1, 2); // 2 ~
    vg.add(1, 3); // 3
    vg.add(1, 4); // 4
    vg.add(2, 5); // 5
    vg.add(2, 6); // 6
    vg.add(3, 4); // 7 ~
    vg.add(5, 6); // 8 ~
    vg.add(3, 7); // 9
    vg.add(6, 8); // 10
    vg.add(7, 8); // 11 ~
    vg.add(8, 9); // 12
    vg.add(9, A); // 13 ~
    vg.add(A, B); // 14
    vg.add(B, C); // 15 ~
    vg.add(C, D); // 16
    vg.mate[1] = vg.mate[2] = 2;
    vg.mate[3] = vg.mate[4] = 7;
    vg.mate[5] = vg.mate[6] = 8;
    vg.mate[7] = vg.mate[8] = 11;
    vg.mate[9] = vg.mate[A] = 13;
    vg.mate[B] = vg.mate[C] = 15;
    int cardinality = vg.max_matching(true);
    assert(cardinality == 7);
}

/**
 *
 *   [D]~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~[E]
 *    |                       |
 *    |                       |
 *   [B]-----[C]              |
 *    ~       ~               |
 *    ~       ~               |
 *   [7]     [8]     [9]~ ~ ~[A]
 *     \     /         \     /
 *      \   /           \   /
 *       \ /             \ /
 *       [5]             [6]
 *        ~               ~
 *       [3]             [4]
 *         \_____   _____/
 *               \ /
 *               [2]
 *                ~
 *               [1]
 *                |
 *               [0]
 *
 * No augmentation path to be found, check blossoms only.
 */
void test_nested_blossom_1() {
    debug_header("check nested blossom 1 vazirani");
    micali_vazirani vg(15);
    vg.add(0, 1); // 0
    vg.add(1, 2); // 1 ~
    vg.add(2, 3); // 2
    vg.add(2, 4); // 3
    vg.add(3, 5); // 4 ~
    vg.add(4, 6); // 5 ~
    vg.add(5, 7); // 6
    vg.add(5, 8); // 7
    vg.add(6, 9); // 8
    vg.add(6, A); // 9
    vg.add(7, B); // 10 ~
    vg.add(8, C); // 11 ~
    vg.add(B, C); // 12
    vg.add(9, A); // 13 ~
    vg.add(A, E); // 14
    vg.add(B, D); // 15
    vg.add(D, E); // 16 ~
    vg.mate[1] = vg.mate[2] = 1;
    vg.mate[3] = vg.mate[5] = 4;
    vg.mate[4] = vg.mate[6] = 5;
    vg.mate[7] = vg.mate[B] = 10;
    vg.mate[8] = vg.mate[C] = 11;
    vg.mate[9] = vg.mate[A] = 13;
    vg.mate[D] = vg.mate[E] = 16;
    int cardinality = vg.max_matching(true);
    assert(cardinality == 7);
}

/**
 *
 *  [B]~ ~ ~ ~ ~ ~ ~ ~ ~ ~[C]
 *   |                     |
 *  [A]                    |
 *   ~                     |     --------[N]
 *  [7]           [8]~ ~ ~[9]   /         ~
 *   |              \_   _/    /         [M]
 *   |                \ /   __/           |
 *  [5]               [6]  /              |
 *   ~                 ~  /    [I]~ ~[J] [K]~[L]
 *  [3]               [4]/       \     \  |  /
 *    \______   ______/           \     \ | /
 *           \ /                   \     \|/
 *           [2]                   [G]   [H]
 *            ~                     ~     ~
 *           [1]                   [E]   [F]
 *            |                       \ /
 *           [0]                      [D]
 *
 * The augmentation path to be found is
 *   0->1->2->3->5->7->A->B->C->9->8->6->4->N->M->K->L->H->F->D.
 * The augmentation path should not go through IJ. The bridge is 4N.
 */
void test_nested_blossom_3() {
    debug_header("check nested blossom 3");
    micali_vazirani vg(24);
    vg.add(0, 1), vg.add(1, 2), vg.add(2, 3), vg.add(2, 4), vg.add(3, 5);
    vg.add(4, 6), vg.add(5, 7), vg.add(6, 8), vg.add(6, 9), vg.add(7, A);
    vg.add(8, 9), vg.add(A, B), vg.add(9, C), vg.add(B, C), vg.add(4, N);
    vg.add(D, E), vg.add(D, F), vg.add(E, G), vg.add(F, H), vg.add(G, I);
    vg.add(H, J), vg.add(H, K), vg.add(H, L), vg.add(I, J), vg.add(K, L);
    vg.add(M, K), vg.add(M, N);
    using int2 = vector<array<int, 2>>;
    int2 m = {{1, 2}, {3, 5}, {4, 6}, {7, A}, {8, 9}, {B, C},
              {E, G}, {F, H}, {I, J}, {K, L}, {N, M}};
    for (auto p : m)
        vg.mate[p[0]] = vg.mate[p[1]] = vg.edge[{p[0], p[1]}];
    int cardinality = vg.max_matching(true);
    assert(cardinality == 12);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_straight_line_1();
    test_straight_line_2();
    test_immediate_blossom_1();
    test_immediate_blossom_2();
    test_immediate_blossom_3();
    test_pyramid_blossom_1();
    test_pyramid_blossom_2();
    test_nested_blossom_1();
    // test_nested_blossom_2();
    test_nested_blossom_3();
    return 0;
}
