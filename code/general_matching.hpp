#ifndef GENERAL_MATCHING_HPP
#define GENERAL_MATCHING_HPP

#include "hash.hpp"
#include "integer_data_structures.hpp"

// *****

/**
 * General maximum matching algorithm of Micali and Vazirani
 * Implemented from scratch, with ideas from:
 * - "Extending Search Phases in the Micali-Vazirani"
 *    Huang M., Stein C.
 *    https://github.com/mh3166/Extended_MV_algorithm
 * - "The General Maximum Matching Algorithm of Micali and Vazirani"
 *    Peterson P., Loui M.
 *
 * Complexity: O(E V^1/2) and O(V + E) memory.
 *
 * Currently passing basic unit tests and random uniform graph tests, being compared with
 * boost's edmonds for correctness and performance.
 *
 * Heavily optimized:
 *   - findpath does not do depth first search
 *   - no hashtables used
 *   - no data structure required to hold a bloom's support
 *   - efficient storage of phaselists and bridges
 *   - two very good greedy maximal matching algorithms that run in O(V + E) time
 *       - node_bootstrap() generally yields a better maximal matching
 *       - edge_bootstrap() is simpler and also good
 *   - extended search phases supported, tunable
 */
struct micali_vazirani {
    int V, E = 0;
    vector<vector<int>> adj;
    vector<array<int, 2>> edge;
    vector<int> mate;

    explicit micali_vazirani(int V = 0) : V(V), adj(V), mate(V, -1) {}

    inline int other(int e, int u) const { return u ^ edge[e][0] ^ edge[e][1]; }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        adj[u].push_back(E);
        adj[v].push_back(E);
        edge.push_back({u, v});
        E++;
    }

    void node_bootstrap() {
        linked_lists buck(V, V);
        vector<int> cnt(V, 0);
        for (int u = 0; u < V; u++) {
            if (mate[u] == -1 && !adj[u].empty()) {
                cnt[u] = adj[u].size();
                buck.push_back(cnt[u], u);
            }
        }
        int s = 1;
        while (s < V) {
            if (buck.empty(s)) {
                s++;
                continue;
            }
            int u = buck.head(s);
            buck.erase(u);
            assert(mate[u] == -1);
            for (int e : adj[u]) {
                int v = other(e, u);
                if (mate[v] == -1) {
                    mate[u] = v, mate[v] = u;
                    buck.erase(v);
                    break;
                }
            }
            if (mate[u] == -1)
                continue;
            for (int w : {u, mate[u]}) {
                for (int e : adj[w]) {
                    int t = other(e, w);
                    if (mate[t] == -1) {
                        cnt[t]--;
                        buck.erase(t);
                        if (cnt[t] > 0) {
                            buck.push_back(cnt[t], t);
                            s = min(s, cnt[t]);
                        }
                    }
                }
            }
        }
    }

    void edge_bootstrap() {
        forward_lists sumbuck(2 * V, E), minbuck(V, E);
        for (int e = 0; e < E; e++) {
            auto [u, v] = edge[e];
            int s = adj[u].size() + adj[v].size();
            sumbuck.push(s, e);
        }
        for (int s = 2 * V - 1; s >= 0; s--) {
            FOR_EACH_IN_FORWARD_LIST (e, s, sumbuck) {
                auto [u, v] = edge[e];
                int m = min(adj[u].size(), adj[v].size());
                minbuck.push(m, e);
            }
        }
        for (int m = 0; m < V; m++) {
            FOR_EACH_IN_FORWARD_LIST (e, m, minbuck) {
                auto [u, v] = edge[e];
                if (mate[u] == mate[v]) // both -1
                    mate[u] = v, mate[v] = u;
            }
        }
    }

    void bootstrap() { node_bootstrap(); }

    int max_matching() {
        int more = 1;
        init();
        while (more && count_matched < V / 2) {
            reset();
            more = search();
        }
        return count_matched;
    }

  private:
    static inline constexpr int inf = INT_MAX / 2;
    static inline constexpr int mv_max_phases = 10; // augmentation phases per search

    struct link_t {
        int hi = -1, lo = -1;
    };

    struct node_t {
        int minlevel = inf, maxlevel = inf, level[2] = {inf, inf};
        int vis = -1, bloom = -1, petal = -1;
        link_t trail[2] = {};
        int arc[2] = {};
        bool color = 0, erased = 0;

        inline void clear() { *this = node_t(); }
    };

    struct bloom_t {
        int peak, base, star;
    };

    struct lists_t {
        vector<int> head, next;

        explicit lists_t(int N = 0, int M = 0) : head(N, -1), next(M) {}

        void push(int h, int n) { next[n] = head[h], head[h] = n; }
        void clear() { fill(begin(head), end(head), -1); }
        void resize(int N, int M) { head.resize(N, -1), next.resize(M); }
    };

    vector<vector<int>> pred, succ;
    vector<node_t> node;
    vector<bloom_t> bloom;
    lists_t phaselist, bridges;
    vector<bool> seen;
    int count_matched, phase, blooms, pending, ddfsid, barrier, exposed[2];

    inline void add_phase(int u, int lvl) { phaselist.push(lvl, u), pending++; }
    inline void add_bridge(int e, int lvl) { bridges.push(lvl, e); }

    // find base*(u) in constant time with union-find
    inline int findstar(int u) {
        int b = node[u].bloom;
        if (b != -1) {
            if (node[bloom[b].star].bloom != -1)
                bloom[b].star = findstar(bloom[b].star);
            return bloom[b].star;
        }
        return u;
    }

    /** Reset routines
     *
     * Add exposed nodes
     */
    void init() {
        pred.resize(V);
        succ.resize(V);
        node.resize(V);
        phaselist.resize(V, V);
        bridges.resize(V, E);

        count_matched = 0;
        for (int u = 0; u < V; u++) {
            pred[u].reserve(adj[u].size());
            succ[u].reserve(adj[u].size());
            assert(mate[u] == -1 || mate[mate[u]] == u);
            count_matched += u < mate[u];
        }
    }

    void reset() {
        phase = blooms = ddfsid = pending = 0;
        phaselist.clear(), bridges.clear();
        for (int u = 0; u < V; u++) {
            pred[u].clear();
            succ[u].clear();
            node[u].clear();
            if (mate[u] == -1) {
                add_phase(u, 0);
                node[u].minlevel = node[u].level[0] = 0;
            }
        }
        seen.assign(E, false);
        bloom.clear();
    }

    int search() {
        bool done = false;
        int more, augmentations = 0, good = 0;
        while (!done && good < mv_max_phases && count_matched < V / 2) {
            done = MIN();
            more = MAX();
            good += more > 0;
            count_matched += more;
            augmentations += more;
            phase++;
        }
        return augmentations;
    }

    /** Breadth first search and MIN
     * For each node with min level equal to phase, find more nodes (previously
     * unseen) and find new bridges.
     *
     * For each node u in the current phaselist
     *   - If the phase is odd, u is inner and we're looking for the matched edge out
     * of u that goes to an unaugmented node through an unprocessed edge.
     *   - If the phase is even, u is outer and we're looking for unmatched edges out
     * of u that go to unaugmented nodes through unprocessed edges.
     *
     * Let the target node be v, and let the edge from u to v be e.
     * - If the minlevel of the node v has not been set or is phase+1 already,
     *   the edge to it is a prop and is part of the primary bfs tree. A successor
     *   and predecessor relationship is recorded for the other subroutines, and v is
     *   added to the next phaselist.
     * - If the minlevel of v has been set and is <=phase then v is already in the
     *   tree and adding the edge e to it as a prop would form a back edge.
     *   Therefore e is not a prop and not part of the tree, hence it is a bridge.
     *
     * For bridges:
     * - If the phase is odd then e is matched and is horizontal (i.e. the minlevel
     *   of v is phase and v is processed in the same iteration as u).
     *   The level of this bridge is known (lvl=phase) and so the bridge can be
     *   saved.
     * - If phase is even however, the edge e is unmatched and it is possible that
     *   the evenlevel of v has not yet been computed. In this case the tenacity/level
     *   of the bridge is unknown at this point. This happens because the even level of
     *   u is smaller than the even level path to v and the bridge is not horizontal,
     *   but actually oblique in the phase graph. This bridge is ignored and will be
     *   processed later, when the even level of v is set.
     */

    void visit_prop(int e, int u, int v, bool parity) {
        assert(!seen[e] && !node[u].erased && !node[v].erased);
        if (node[v].minlevel == inf) {
            node[v].minlevel = node[v].level[!parity] = phase + 1;
            add_phase(v, phase + 1);
        }
        assert(node[v].minlevel == phase + 1 && node[v].level[!parity] == phase + 1);
        pred[v].push_back(u);
        succ[u].push_back(v);
        seen[e] = true;
    }

    void visit_bridge(int e, bool parity) {
        auto [u, v] = edge[e];
        assert(!seen[e] && !node[u].erased && !node[v].erased);
        assert(node[u].level[parity] < inf && node[v].level[parity] < inf);
        int tenacity = node[u].level[parity] + node[v].level[parity] + 1;
        int lvl = tenacity >> 1;
        assert(phase <= lvl && lvl < V);
        add_bridge(e, lvl);
        seen[e] = true;
    }

    void bfs_visit(int e, int u, int v) {
        bool parity = phase % 2;
        if (node[v].minlevel > phase) {
            visit_prop(e, u, v, parity);
        } else if (node[v].level[parity] < inf) {
            visit_bridge(e, parity);
        }
    }

    bool MIN() {
        if (pending == 0)
            return true;

        bool parity = phase % 2;
        for (int u = phaselist.head[phase]; u != -1; pending--, u = phaselist.next[u]) {
            if (node[u].erased)
                continue;
            if (parity == 0) {
                for (int e : adj[u]) {
                    int v = other(e, u);
                    if (mate[u] != v && !seen[e] && !node[v].erased)
                        bfs_visit(e, u, v);
                }
            } else if (!node[mate[u]].erased) {
                for (int e : adj[u]) {
                    int v = other(e, u);
                    if (mate[u] == v && !seen[e])
                        bfs_visit(e, u, v);
                    if (mate[u] == v)
                        break;
                }
            }
        }
        return false;
    }

    /** Double depth first search and MAX
     * For a given bridge 'e' with endpoints 'red' and 'blue', either find a bloom, or
     * find an augmenting path, or do nothing and exit immediately.
     *
     * The idea is straightforward: we maintain two dfs "trails" instead of just one.
     * The goal is to arrive at two distinct exposed nodes and thus identify an
     * augmenting path made up of the concatenation of both trails, but this will be
     * impossible in the presence of a bloom.
     *
     * The two trails are colored red and blue, and along the ddfs the nodes visited
     * are appropriately colored. Predecessor relationships are stored, which maintain
     * the trail data attached to the nodes.
     *
     * The trails cannot enter into nodes visited by the other trail, identified by their
     * color, even if the other trail has meanwhile backtracked out of that node. This
     * guarantees the two trails never intersect. In practice, the implementation
     * maintains two trail heads r and b, and the parent trail pointers are attached to
     * the nodes as they are visited. The two trail heads may collide, at which point
     * backtracking is performed on b first by convention.
     *
     * Along the ddfs we may need to backtrack along one of the two trails. Being
     * optimistic, while advancing we can just take predecessor edges at will and hope we
     * don't collide the two trails, and then "backtrack" one of the two trails in case of
     * collision, by convention b. If the first backtracking fails, we try the second
     * trail, and if that fails we found a "barrier" node that is a common ancestor of all
     * the nodes visited by the ddfs (and the ones skipped over). Indeed all prop edges
     * emanating from this barrier node are visited, to ensure there is no way around it.
     * Backtracking succeeds if the trail can get "around" the collision point, and fails
     * otherwise. Getting "around" means finding another reachable node at the same or
     * lower depth than the collision point. In case of failure the trail head is commited
     * to the collision point and its other trail's turn to backtrack.
     *
     * To backtrack efficiently we maintain an arc index attached to each of the nodes,
     * which identify the next predecessor to take at that node after backtracking to it
     * along a trail. If the index is invalid, there are no more predecessors and further
     * backtracking is needed.
     *
     * Saved data:
     * The augmenting path subroutine will need to reuse the colored trails found during
     * ddfs, going backwards from the found exposed nodes. The blooms formed also maintain
     * predecessor (petal) and successor (trail) node relationships. Therefore to traverse
     * the bloom we just need to follow the pointers, and no dfs is required.
     *
     * The bloom formation subroutine will need to perform dfs again (this time with
     * no major fuss) to assign maxlevels, bloom ids and identify new bridges. It will
     * also need to form two primary colored petals starting from the base.
     *
     * Nothing is done by ddfs if red and blue were already in blooms with the same base*.
     *
     * Observation 1 (correctness optimization)
     * - A node Q is unerased iff it has not been used in an augmenting path
     *   and there is still a path from it to an exposed vertex.
     * - After augmentations, function erase_successors() erases nodes that have
     *   become unreachable from exposed vertices. Our ddfs implementation requires
     *   this procedure to be implemented properly - when advancing into a node u, there
     *   must be some path from that node to an exposed node.
     * - The predecessors of a node we're going to advance to can be stored in
     *   arbitrary order. As such, as we iterate over the predecessor array of a node u,
     *   we can eliminate nodes that have been erased by swapping them with the last
     *   element.
     * *
     * ***** Data structures
     *
     * * Three pointers: r (red, left), b (blue, right), barrier (for b).
     * * One arc index per node
     * * Four trail pointers per node, two for each color.
     *   Given a node u, if the trail head is at u and a predecessor edge to v is taken,
     *   and w=base*(v) is the next trail head, then we set
     *       w.trail.hi:=u,   w.trail.lo:=v.   w.arc:=index of next(v)
     *   Colored trail pointers are necessary for the bloom bases.
     *
     * ***** Algorithm
     *
     * - Advance whichever of r and b is further from the roots (i.e. largest min_level)
     *   advancing r in case of a tie.
     *   - When advancing into nodes that belong to blooms skip directly to the
     *     bloom's base star (recursive base).
     *   - Be careful not to advance into nodes that have been erased (by a previous
     *     augmentation in the same search) (observation 2)
     *   - Notice that advancing casually (i.e. not while backtracking) only one step
     *     at a time guarantees the two paths can only cross at their endpoints (when
     *     r==b) and not somewhere in between. This is not necessarily true while
     *     backtracking either r or b as we usually need to backtrack and readvance
     *     several steps.
     *
     * - If r == b meet at the same node, this node is a bottleneck.
     *   - Backtrack b first. Repeat, until there are no more nodes to search or the
     *     trail has gone around the bottleneck:
     *     - Pop nodes from the trail until one has more predecessor nodes to go to,
     *     or until we reach the barrier.
     *     - Advance the trail greedily along predecessor nodes, until the head collides
     *     with the other trail, revisits an old node, or goes around the trail.
     *     - Backtracking fails when the head is equal to the barrier and there are no
     *     more predecessor edges; otherwise it succeeds. In this case, set barrier to
     *     this bottleneck and backtrack r.
     *
     * - If at any point both r!=b have no predecessors, i.e. they are exposed roots,
     *   we are done and found an augmenting path.
     * - If neither trail can get around the bottleneck, the bottleneck is the base of
     *   a new bloom.
     */

    void push_dfs(int& h, bool c, int v, int w) {
        assert(node[h].vis != ddfsid || (node[h].color == c && node[h].arc[c] > 1));
        node[h].vis = ddfsid;
        node[h].color = c;
        node[w].arc[c] = 0;
        node[w].trail[c].hi = h;
        node[w].trail[c].lo = v;
        h = w;
    }

    bool pop_dfs(int& h, bool c) {
        h = node[h].trail[c].hi;
        int i = node[h].arc[c], s = pred[h].size();
        return i < s;
    }

    void advance_dfs(int& h, bool c) {
        int& i = node[h].arc[c];
        int v = pred[h][i++], w = findstar(v);
        lazy_erase_predecessors(h, i);
        push_dfs(h, c, v, w);
    }

    bool reverse_dfs(int& h, bool c, int b) {
        bool ok = false;
        while (!ok && h != b) {
            ok = pop_dfs(h, c);
        }
        return ok;
    }

    bool backtrack_dfs(int& h, bool c, int b) {
        int x = h, lvl = node[x].minlevel;
        while ((h == x || node[h].minlevel > lvl) && reverse_dfs(h, c, b)) {
            do {
                advance_dfs(h, c);
            } while (node[h].vis != ddfsid && node[h].minlevel > lvl);
        }
        return h != b;
    }

    int ddfs(int peak) {
        int r = findstar(edge[peak][0]), b = findstar(edge[peak][1]);
        if (r == b)
            return -1;

        int red_barrier = r;
        ++ddfsid, barrier = b;
        node[r].arc[0] = node[b].arc[1] = 0;
        node[r].trail[0] = node[b].trail[1] = {};

        while (node[r].minlevel != 0 || node[b].minlevel != 0) {
            if (node[r].minlevel >= node[b].minlevel) {
                advance_dfs(r, 0);
            } else {
                advance_dfs(b, 1);
            }
            if (r == b) {
                if (!backtrack_dfs(b, 1, barrier)) {
                    b = barrier = r;
                    if (!backtrack_dfs(r, 0, red_barrier)) {
                        r = barrier = b;
                        return 0;
                    }
                }
            }
        }
        assert(r != b);
        exposed[0] = r, exposed[1] = b;
        return 1;
    }

    int MAX() {
        int augmentations = 0;
        for (int peak = bridges.head[phase]; peak != -1; peak = bridges.next[peak]) {
            auto [red, blue] = edge[peak];
            if (node[red].erased || node[blue].erased)
                continue;

            auto what = ddfs(peak);
            if (what == 1) {
                augment_path(peak);
                augmentations++;
            } else if (what == 0) {
                form_bloom(peak);
                blooms++;
            }
        }
        return augmentations;
    }

    /** Bloom formation and coloring
     *
     *              red  (blue)          walk examples, without the expanded blooms:
     *            ____R  (B)___
     *       ____/   / \  |\   \___             - walk_peak (using bloom succ):
     *      /       /   \ | \      \            p->i->e->B       (blue)
     *     /       /     \|  \      \           q->x->c->B       (blue)
     *    a       b      (c) (d)    (e)         r->n->h->b->R     red
     *    |\_   _/|\       \ / \___  |
     *    |  \_/  | \___   (x)     \ |          - walk_base (using bloom pred):
     *    |__/ \__|     \  / \      \|          R->b->h->n->t     red
     *    f       g      h/   \     (i)         B->c->x->q       (blue)
     *    |      /|    _/|\    \     |
     *    |     / |  _/ /| \    \    |          - walk_down (using whatever):
     *    |    /  |_/  / |  \    \   |          g->l->q
     *    j   k   l   /  m   n   (o)(p)         a->f->j->q
     *     \   \  |  /   |  /|  _/  /           d->i->p->t
     *      \   \ | /   _|_/ | /   /
     *       \___\|/   / |   |/___/         bloom succ of b: R
     *           (q)  r  s   t              bloom succ of c: B
     *             \  |  |  /               bloom succ of i: d or e, doesn't matter
     *              \ |  | /                bloom pred of h: n
     *               [base]                 bloom pred of c: x
     *
     * ***** After finding a bloom
     * If a bloom is found we identify:
     *     - the peak bridge (e)
     *     - the base (barrier)
     *
     * All nodes reachable from the peak through predecessor edges up to the barrier
     * have been visited and properly colored. All these nodes constitute the bloom.
     * Every node u in this bloom has been colored red or blue such that:
     *   a) If u is red  there is a red  path from u to the red  peak node.
     *      If u is blue there is a blue path from u to the blue peak node.
     *   b) There is a red  path from the red  peak to the base (primary red  petal).
     *      There is a blue path from the blue peak to the base (primary blue petal).
     *   c) If u is red there  isn't necessarily a red  path from u to the base.
     *      If u is blue there isn't necessarily a blue path from u to the base.
     *
     * a) This invariant allow us to find a path from any node u to a peak using only
     * one color. If an augmenting path is found through the bloom that needs to go
     * "around" the bloom, this path is used to go from the first node to the peak,
     * making sure to use only one color as another disjoint path going down must be
     * found too. We optimistically assume such an augmenting path will be found, so
     * to facilitate its construction, we assign bloom successors to every node
     * alongside their maxlevels. If P(u) is a path of u's color from u to the peak,
     * then the bloom successor of u is the "guide" prop edge e that leads to the
     * second node in P(u). Suppose ddfs, to reach u, went from vertex v through edge
     * e, pointing to w. Then u = base*(w) and the successor prop of u is edge e. The
     * second node in P(u) is v, before expanding blooms. Storing the edge e instead
     * of v allows us to know the vertex w immediately to expand the blooms up to u.
     *
     * b) These paths correspond to the red and blue trails constructed during ddfs.
     * We didn't store these, but we can recover them quickly with a naive dfs
     * following predecessors of the same color. If an augmenting path is found
     * through the bloom that needs to go "around" the bloom, this path is used to go
     * down from the peak to the base, making sure to use the opposite color of that
     * used to go from the first node to the peak. To identify this "primary petal" we
     * assign bloom predecessors to the nodes on it. Only one such petal is
     * constructed for each color.
     *
     * c) This happens because the ddfs might backtrack successfully more than one
     * node, leaving for example a blue node with only red predecessor nodes. Such
     * nodes were removed from the trail but are still part of the bloom. If an
     * augmenting path is found through the bloom that needs to "descend" the bloom
     * directly, i.e. not go "around" it, then the colors don't matter as only a
     * single subpath through the bloom is needed, not two disjoint ones.
     *
     * Be careful not to touch the bloom's base in any way.
     *
     * ***** Algorithm
     *
     * - To form the primary red and blue petals, walk the trail pointers starting at
     *   the base. Both color trails have been filled.
     * - Perform dfs to assign maxlevels, bloom ids, and identify bridges.
     *   - Visit only pred nodes of the same color.
     *   - Avoid revisiting nodes reachable in multiple ways.
     *   - For bloom nodes with even maxlevel, look for bridges out of the bloom.
     */

    void bloom_build_petals() {
        for (bool c : {0, 1}) {
            int u = barrier, v = node[u].trail[c].hi;
            while (v != -1) {
                node[v].petal = node[u].trail[c].lo;
                u = v, v = node[u].trail[c].hi;
            }
        }
    }

    void bloom_dfs_level(int u) {
        if (u == barrier)
            return;

        int lvl = 2 * phase + 1 - node[u].minlevel;
        assert(!node[u].erased && node[u].bloom == -1 && node[u].vis == ddfsid);
        assert(lvl < V && node[u].maxlevel == inf && node[u].level[lvl % 2] == inf);

        node[u].bloom = blooms;
        node[u].maxlevel = node[u].level[lvl % 2] = lvl;
        add_phase(u, lvl);

        for (int v : pred[u]) {
            int w = findstar(v);
            if (node[w].bloom == -1 && node[u].color == node[w].color)
                bloom_dfs_level(w);
        }

        if (lvl % 2 == 0) {
            for (int e : adj[u]) {
                int v = other(e, u);
                if (!node[v].erased && !seen[e] && node[v].level[0] < inf)
                    visit_bridge(e, 0);
            }
        }
    }

    void form_bloom(int peak) {
        assert(node[barrier].bloom == -1);
        bloom.push_back({peak, barrier, barrier});
        bloom_build_petals();
        bloom_dfs_level(findstar(edge[peak][0]));
        bloom_dfs_level(findstar(edge[peak][1]));
    }

    /** Path augmentation and bloom expansion
     *
     *              a
     *               \
     *                \  5~ ~ ~6
     *                 \ |     |    b            going down the bloom:
     *                  \|     |   /               a-->3-->1-->base
     *                   3     4  /                   \_____/
     *                   ~     ~ /                   walk_down()
     *                   ~     ~/
     *                   1     2                 going around the bloom:
     *                    \   /                    b-->2-->4-->6-->5-->3-->1-->base
     *                     \ /                        \_________/ \_________/
     *                     base                        walk_peak() walk_base()
     *
     * ***** After finding an augmenting path
     * If an augmenting path is found we identify:
     *     - the peak bridge (e)
     *     - the red and blue trails, properly colored, including exposed nodes
     *       the trails can be walked starting at the exposed nodes
     *
     *   Each trail red/blue identifies a sequence of nodes t=u(0),u(1),u(2),...,u(n)
     * such that t is the red/blue peak and u(n) is an exposed vertex. The two
     * sequences are disjoint by construction.
     *
     * ***** Algorithm
     *
     * - For each trail red/blue with color c and top t:
     *   - Expand the initial bloom-star of t, if it exists.
     *   - Walk the trail, starting at u(n). Expand the bloom-star contracted in
     *                        [trail[u(i)][c].lo, u(i)],
     *     then append u(i), for i=n,n-1,...,1, and finally append t.
     *   - Be careful to maintain the path's correct direction throughout.
     * - Join the two expanded paths.
     * - Augment the constructed path by inverting all edges along it.
     * - Erase all nodes in the path and all of their successors who can no longer
     *   be reached by any exposed node, recursively.
     *
     * ***** Node erasure
     *
     * - Using lazy erasure, erase only the first element of the successor's pred list
     * until it is an unerased node. If the pred list becomes empty as a result the
     * successor is to be erased as well.
     * - Use lazy erasure also in ddfs to avoid entering erased nodes.
     */

    using path_t = list<int>;

    inline void add_path(path_t& path, bool back, path_t&& subpath) {
        path.splice(back ? end(path) : begin(path), subpath);
    }
    inline void add_path(path_t& path, bool back, int node) {
        back ? path.push_back(node) : path.push_front(node);
    }

    path_t walk_bloom(int u, bool down) {
        int B = node[u].bloom;

        if (node[u].minlevel % 2 == 0) /* outer */ {
            return walk_down(u, B, down);
        } else /* inner */ {
            int t = edge[bloom[B].peak][!node[u].color];
            auto path = walk_peak(u, B, !down);
            add_path(path, down, walk_base(t, B, down));
            return path;
        }
    }

    path_t walk_star(int u, int star, bool down) {
        path_t path;
        while (u != star) {
            add_path(path, down, walk_bloom(u, down));
            u = bloom[node[u].bloom].base;
        }
        return path;
    }

    path_t walk_peak(int u, int B, bool down) {
        bool c = node[u].color;
        int t = edge[bloom[B].peak][c];

        path_t path{u}, top_path;
        while (node[t].bloom != B) {
            add_path(top_path, down, walk_bloom(t, down));
            t = bloom[node[t].bloom].base;
        }
        while (u != t) {
            int v = node[u].trail[c].lo;
            add_path(path, !down, walk_star(v, u, down));
            u = node[u].trail[c].hi;
            add_path(path, !down, u);
        }
        add_path(path, !down, move(top_path));
        return path;
    }

    path_t walk_base(int u, int B, bool down) {
        int base = bloom[B].base;

        path_t path;
        while (u != base) {
            if (node[u].bloom == B) {
                add_path(path, down, u);
                u = node[u].petal; // take a predecessor of the same color
            } else {
                add_path(path, down, walk_bloom(u, down));
                u = bloom[node[u].bloom].base;
            }
        }
        return path;
    }

    path_t walk_down(int u, int B, bool down) {
        int base = bloom[B].base;

        path_t path;
        while (u != base) {
            if (node[u].bloom == B) {
                add_path(path, down, u);
                u = pred[u].front(); // any predecessor works to go down
            } else {
                add_path(path, down, walk_bloom(u, down));
                u = bloom[node[u].bloom].base;
            }
        }
        return path;
    }

    path_t find_path(int top, int c, bool down) {
        int u = exposed[c], w = findstar(top);

        path_t path;
        while (u != w) {
            int v = node[u].trail[c].lo;
            add_path(path, !down, u);
            add_path(path, !down, walk_star(v, u, down));
            u = node[u].trail[c].hi;
        }
        add_path(path, !down, w);
        add_path(path, !down, walk_star(top, w, down));
        return path;
    }

    void augment_path(int peak) {
        auto path = find_path(edge[peak][0], 0, false);
        auto rest = find_path(edge[peak][1], 1, true);
        path.splice(end(path), rest);

        assert(path.size() == 2u * phase + 2);

        auto ait = begin(path), bit = next(ait);
        while (bit != end(path)) {
            int u = *ait++, v = *bit++;
            assert(!node[u].erased && !node[v].erased);
            if (mate[v] != u)
                mate[u] = v, mate[v] = u;
        }
        for (int u : path) {
            node[u].erased = true;
        }
        erase_successors(path);
    }

    void erase_successors(path_t& path) {
        while (!path.empty()) {
            int u = path.front();
            path.pop_front();
            for (int v : succ[u]) {
                if (!node[v].erased && lazy_erase_predecessors(v)) {
                    node[v].erased = true;
                    path.push_back(v);
                }
            }
        }
    }

    inline bool lazy_erase_predecessors(int v, int i = 0) {
        int s = pred[v].size();
        while (i < s && node[pred[v][i]].erased) {
            swap(pred[v][i], pred[v].back()), pred[v].pop_back(), s--;
        }
        return i == s; // erased all after i?
    }
};

#endif // GENERAL_MATCHING_HPP
