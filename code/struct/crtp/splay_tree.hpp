#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Binary splay tree ready for CRTP subclassing.
 */
template <typename Splay>
struct splay_tree_base {
    Splay* parent = nullptr;
    Splay* child[2] = {};

  protected:
    splay_tree_base() = default;
    void pushdown() {}
    void pushup() {}

  private:
    Splay* self() { return static_cast<Splay*>(this); }
    const Splay* self() const { return static_cast<const Splay*>(this); }

    bool is_root() const { return !parent; }
    bool is_left() const { return parent && self() == parent->child[0]; }
    bool is_right() const { return parent && self() == parent->child[1]; }
    static void adopt(Splay* parent, Splay* child, int8_t side) {
        if (side >= 0)
            parent->child[side] = child;
        if (child)
            child->parent = parent;
    }

    void rotate() {
        Splay *p = parent, *g = p->parent;
        bool side = self() == p->child[1];
        adopt(p, child[!side], side);
        adopt(g, self(), g ? p == g->child[1] : -1);
        adopt(self(), p, !side);
        p->pushup();
    }

  public:
    Splay* root() { return parent ? parent->root() : self(); }
    Splay* min_node() { return child[0] ? child[0]->min_node() : self(); }
    Splay* max_node() { return child[1] ? child[1]->max_node() : self(); }
    Splay* next() {
        if (child[1])
            return child[1]->min_node();
        Splay* node = self();
        while (node && !node->is_left())
            node = node->parent;
        return node;
    }
    Splay* prev() {
        if (child[0])
            return child[0]->min_node();
        Splay* node = self();
        while (node && !node->is_right())
            node = node->parent;
        return node;
    }

    void splay() {
        while (parent && parent->parent) {
            parent->parent->pushdown(), parent->pushdown(), self()->pushdown();
            bool zigzig = is_right() == parent->is_right();
            (zigzig ? parent : self())->rotate(), rotate();
        }
        if (parent) {
            parent->pushdown(), self()->pushdown(), rotate();
        }
        self()->pushdown(), self()->pushup();
    }

    void insert_node(Splay* c, int side) {
        assert(c && !child[side]);
        adopt(self(), c, side);
        c->pushup(), c->splay();
    }

    void insert_before(Splay* c) {
        if (!child[0]) {
            self()->insert_node(c, 0);
        } else {
            child[0]->max_node()->insert_node(c, 1);
        }
    }

    void insert_after(Splay* c) {
        if (!child[1]) {
            self()->insert_node(c, 1);
        } else {
            child[1]->min_node()->insert_node(c, 0);
        }
    }

    Splay* detach_left() {
        assert(is_root());
        self()->pushdown();
        Splay* subtree = child[0];
        if (subtree) {
            subtree->parent = nullptr;
            child[0] = nullptr;
            self()->pushup();
            return subtree;
        }
        return nullptr;
    }

    Splay* detach_right() {
        assert(is_root());
        self()->pushdown();
        Splay* subtree = child[1];
        if (subtree) {
            subtree->parent = nullptr;
            child[1] = nullptr;
            self()->pushup();
            return subtree;
        }
        return nullptr;
    }

    Splay* split_before() { return splay(), detach_left(); }
    Splay* split_after() { return splay(), detach_right(); }

    // L and R must be roots or null
    static Splay* join(Splay* L, Splay* R) {
        if (!L || !R) {
            return L ? L : R;
        }
        assert(L->is_root() && R->is_root());
        Splay* root = L->max_node();
        root->splay();
        adopt(root, R, 1);
        root->pushup();
        return root;
    }

    static Splay* lca(Splay* u, Splay* v) {
        assert(u && v);
        Splay *a = u, *b = v;
        while (a != b) {
            a = a->parent, b = b->parent;
            if (!a)
                a = v;
            if (!b)
                b = u;
        }
        return a;
    }
};

/**
 * CRTP #1 add order statistics to the splay tree nodes
 */
template <typename Splay>
struct splay_order : splay_tree_base<Splay> {
    int sz = 1;

  protected:
    friend splay_tree_base<Splay>;
    static int get_size(const Splay* s) { return s ? s->sz : 0; }
    void pushup() { sz = 1 + get_size(self()->child[0]) + get_size(self()->child[1]); }

  private:
    Splay* self() { return static_cast<Splay*>(this); }
    const Splay* self() const { return static_cast<const Splay*>(this); }

  public:
    Splay* find_by_order(int order) {
        if (order >= sz) {
            return nullptr;
        }
        Splay* node = self();
        while (true) {
            if (order == get_size(node->child[0])) {
                return node;
            } else if (order < get_size(node->child[0])) {
                node = node->child[0];
            } else {
                order -= get_size(node->child[0]) + 1;
                node = node->child[1];
                assert(node);
            }
        }
    }

    int order_of_node() const {
        const Splay *node = self(), *parent = node->parent;
        int order = get_size(node->child[0]);
        while (parent) {
            if (parent->child[1] == node) {
                order += get_size(parent->child[0]) + 1;
            }
            node = parent, parent = node->parent;
        }
        return order;
    }

    int size() const { return sz; }
};
