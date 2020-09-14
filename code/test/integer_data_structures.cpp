#include "../integer_data_structures.hpp"

#include "../debug_print.hpp"

// *****

void test_forward_lists() {
    forward_lists fl(6, 20);
    //                 0  1  2  3  4  5
    fl.view(5, 1); //                 v
    fl.push(2, 3); //  3     3
    fl.push(2, 4); //  4     4
    fl.push(2, 7); //  7     7
    fl.copy(0, 2); //  ^
    fl.push(2, 9); //        9
    fl.push(3, 5); //           5     5
    fl.push(4, 8); //              8
    fl.push(3, 1); //           1     1
    fl.push(2, 0); //        0
    fl.push(1, 2); //     2
    fl.push(3, 6); //           6

    vector<vector<int>> seen(6);
    for (int l = 0; l < 6; l++)
        FOR_EACH_IN_FORWARD_LIST (i, l, fl)
            seen[l].push_back(i);

    print("forward_lists\n");
    debugh(seen);
    assert(seen[0] == vector<int>({7, 4, 3}));
    assert(seen[1] == vector<int>({2}));
    assert(seen[2] == vector<int>({0, 9, 7, 4, 3}));
    assert(seen[3] == vector<int>({6, 1, 5}));
    assert(seen[4] == vector<int>({8}));
    assert(seen[5] == vector<int>({1, 5}));
}

void test_linked_lists() {
    linked_lists ll(5, 20);
    ll.push_back(0, 1);
    ll.push_back(0, 2);
    ll.push_front(0, 3); // 0: (3->1->2)

    ll.push_front(1, 4);
    ll.push_back(3, 5);
    ll.splice_back(1, 3);
    ll.push_back(3, 6);
    ll.splice_after(5, 3);
    ll.push_front(1, 7); // 1: (7->4->5->6)

    ll.splice_after(1, 1); // 0: (3->1->7->4->5->6->2),  1: ()

    ll.push_front(2, 8);
    ll.push_front(2, 9);
    ll.push_back(2, 10);     // 2: (9->8->10)
    ll.insert_before(8, 11); // 2: (9->11->8->10)
    ll.insert_after(8, 12);  // 2: (9->11->8->12->10)

    ll.push_back(3, 13);
    ll.push_back(3, 14);
    ll.push_back(3, 15); // 3: (13->14->15)

    // 4 is empty here
    ll.splice_before(14, 4);
    ll.splice_after(14, 4);
    ll.splice_before(13, 4);
    ll.splice_after(15, 4);
    ll.splice_front(3, 4);
    ll.splice_back(3, 4); // 3: (13->14->15)

    ll.splice_before(9, 3); // 2: (13->14->15->9->11->8->12->10)
    ll.push_front(3, 16);
    ll.push_front(3, 17);
    ll.splice_back(4, 3); // move 4 to 3
    ll.push_front(4, 18); // 4: (18->17->16)
    ll.splice_back(2, 4); // 2: (13->14->15->9->11->8->12->10->18->17->16)
    ll.push_back(4, 19);
    ll.splice_before(10, 4); // 2: (13->14->15->9->11->8->12->19->10->18->17->16)

    vector<vector<int>> fw(5), bw(5);
    for (int l = 0; l < 5; l++) {
        FOR_EACH_IN_LINKED_LIST (i, l, ll)
            fw[l].push_back(i);
        FOR_EACH_IN_LINKED_LIST_REVERSE (i, l, ll)
            bw[l].push_back(i);
    }

    print("linked_lists\n");
    debugh(fw), debugh(bw);
    assert(fw[0] == vector<int>({3, 1, 7, 4, 5, 6, 2}));
    assert(fw[1] == vector<int>({}));
    assert(fw[2] == vector<int>({13, 14, 15, 9, 11, 8, 12, 19, 10, 18, 17, 16}));
    assert(fw[3] == vector<int>({}));
    assert(fw[4] == vector<int>({}));
    assert(bw[0] == vector<int>({2, 6, 5, 4, 7, 1, 3}));
    assert(bw[1] == vector<int>({}));
    assert(bw[2] == vector<int>({16, 17, 18, 10, 19, 12, 8, 11, 9, 15, 14, 13}));
    assert(bw[3] == vector<int>({}));
    assert(bw[4] == vector<int>({}));
}

void test_ondemand_lists() {
    ondemand_lists<linked_lists> ll(6, 20);

    int a = ll.demand(); // 0
    assert(a == 0);
    ll.push_back(a, 1);
    ll.push_back(a, 2);
    ll.push_front(a, 3); // 0: (3->1->2)

    int b = ll.demand(); // 1
    int c = ll.demand(); // 2
    assert(b == 1 && c == 2);
    ll.push_front(b, 4);
    ll.push_back(c, 5);
    ll.splice_back(b, c);
    ll.push_back(c, 6);
    ll.splice_after(5, c);
    ll.push_front(b, 7); // 1: (7->4->5->6)
    ll.clear(c);

    ll.splice_after(b, 1); // 0: (3->1->7->4->5->6->2),  b: ()

    c = ll.demand(); // 2
    assert(c == 2);
    ll.push_front(c, 8);
    ll.push_front(c, 9);
    ll.push_back(c, 10);     // 2: (9->8->10)
    ll.insert_before(8, 11); // 2: (9->11->8->10)
    ll.insert_after(8, 12);  // 2: (9->11->8->12->10)

    int d = ll.demand(); // 3
    assert(d == 3);
    ll.push_back(d, 13);
    ll.push_back(d, 14);
    ll.push_back(d, 15); // 3: (13->14->15)

    // e is empty here
    int e = ll.demand(); // 4
    assert(e == 4);
    ll.splice_before(14, e);
    ll.splice_after(14, e);
    ll.splice_before(13, e);
    ll.splice_after(15, e);
    ll.splice_front(d, e);
    ll.splice_back(d, e); // 3: (13->14->15)

    ll.splice_before(9, d); // 2: (13->14->15->9->11->8->12->10)
    ll.push_front(d, 16);
    ll.push_front(d, 17);
    ll.splice_back(e, d); // move 4 to 3
    ll.push_front(e, 18); // 4: (18->17->16)
    ll.splice_back(c, e); // 2: (13->14->15->9->11->8->12->10->18->17->16)
    ll.push_back(e, 19);
    ll.splice_before(10, e); // 2: (13->14->15->9->11->8->12->19->10->18->17->16)

    vector<vector<int>> fw(5), bw(5);
    for (int l = 0; l < 5; l++) {
        FOR_EACH_IN_LINKED_LIST (i, l, ll)
            fw[l].push_back(i);
        FOR_EACH_IN_LINKED_LIST_REVERSE (i, l, ll)
            bw[l].push_back(i);
    }

    print("ondemand_lists<linked_lists>\n");
    debugh(fw), debugh(bw);
    assert(fw[0] == vector<int>({3, 1, 7, 4, 5, 6, 2}));
    assert(fw[1] == vector<int>({}));
    assert(fw[2] == vector<int>({13, 14, 15, 9, 11, 8, 12, 19, 10, 18, 17, 16}));
    assert(fw[3] == vector<int>({}));
    assert(fw[4] == vector<int>({}));
    assert(bw[0] == vector<int>({2, 6, 5, 4, 7, 1, 3}));
    assert(bw[1] == vector<int>({}));
    assert(bw[2] == vector<int>({16, 17, 18, 10, 19, 12, 8, 11, 9, 15, 14, 13}));
    assert(bw[3] == vector<int>({}));
    assert(bw[4] == vector<int>({}));
}

void test_int_forest() {
    int N = 15;
    int_forest f(N);
    f.place(0);
    f.place(1);
    f.place(2); // N(0,1,2)
    f.splice(1, 3);
    f.splice(1, 4); // N(0,1(3,4),2)
    f.splice(2, 5);
    f.splice(2, 6); // N(0,1(3,4),2(5,6))
    f.splice(0, 2); // N(0(2(5,6)),1(3,4))
    f.splice_before(6, 7);
    f.splice_after(3, 8); // N(0(2(5,7,6)),1(3,8,4))
    f.drop(5);
    f.place(5);
    f.place(9); // N(0(2(7,6)),1(3,8,4),5,9)
    f.splice(9, 10);
    f.splice(9, 11);
    f.splice(9, 12);     // N(0(2(7,6)),1(3,8,4),5,9(10,11,12))
    f.swap_places(3, 8); // N(0(2(7,6)),1(8,3,4),5,9(10,11,12))
    f.swap_places(9, 6); // N(0(2(7,9(10,11,12))),1(8,3,4),5,6)
    f.promote(11);       // N(0(2(7,11(10,9,12))),1(8,3,4),5,6)
    f.splice(2, 13);     // N(0(2(7,11(10,9,12),13)),1(8,3,4),5,6)
    f.splice(0, 14);     // N(0(2(7,11(10,9,12),13),14),1(8,3,4),5,6)
    f.splice(8, 11);     // N(0(2(7,13),14),1(8(11(10,9,12)),3,4),5,6)
    f.swap_places(0, 1);
    f.swap_places(6, 5); // N(1(8(11(10,9,12)),3,4),0(2(7,13),14),6,5)

    vector<vector<int>> par(N + 1), child(N + 1);

    for (int n = 0; n <= N; n++) {
        if (f.contains(n))
            FOR_EACH_CHILD_INT_TREE (i, n, f)
                child[n].push_back(i);
        if (f.contains(n))
            FOR_EACH_ANCESTOR_INT_TREE (i, n, f)
                par[n].push_back(i);
    }

    print("int_forest\n");
    diota("u", f.S);
    debug(f.parent);
    debug(f.next);
    debug(f.prev);
    debugh(par), debugh(child);
}

int main() {
    test_forward_lists();
    test_linked_lists();
    test_ondemand_lists();
    test_int_forest();
    return 0;
}
