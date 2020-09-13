#ifndef GENERAL_MATCHING_HPP
#define GENERAL_MATCHING_HPP

#include "hash.hpp"

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
 * Complexity: O(E V^1/2) with a monstrous constant.
 *
 * Currently passing unit tests and random tests, compared with boost's edmonds for
 * correctness and performance. Better than edmonds for V>50.
 */
struct micali_vazirani {
    int V, E = 0;
    vector<vector<int>> adj;
    vector<int> source, target, mate;
    unordered_map<pair<int, int>, int, pair_hasher> edge;

    explicit micali_vazirani(int V = 0) : V(V), adj(V), mate(V, -1) {}

    int other(int e, int u) { return u ^ source[e] ^ target[e]; }
    int get(int u, int v) const { return edge.at(minmax(u, v)); }
    bool has_edge(int u, int v) const { return edge.count(minmax(u, v)); }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        auto e = minmax(u, v);
        assert(!edge.count(e));
        adj[u].push_back(v);
        adj[v].push_back(u);
        source.push_back(u);
        target.push_back(v);
        edge[e] = E++;
    }

    void bootstrap() {
        vector<int> edges(E);
        iota(begin(edges), end(edges), 0);
        sort(begin(edges), end(edges), [&](int a, int b) {
            return adj[target[a]].size() < adj[target[b]].size();
        });
        stable_sort(begin(edges), end(edges), [&](int a, int b) {
            return adj[source[a]].size() < adj[source[b]].size();
        });
        for (int i = 0; i < E; i++) {
            int e = edges[i], u = source[e], v = target[e];
            if (mate[u] == mate[v])
                mate[u] = mate[v] = e;
        }
    }

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

    struct node_t {
        int minlevel, maxlevel, level[2];
        int vis, bloom, bloom_pred, bloom_succ;
        bool color, erased;
        vector<int> pred, succ;

        inline void clear() {
            minlevel = level[0] = inf;
            maxlevel = level[1] = inf;
            vis = bloom = bloom_pred = bloom_succ = -1;
            color = erased = false;
            pred.clear(), succ.clear();
        }
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

    vector<node_t> node;   // node data: level, preds, succs, bloom, color...
    vector<bloom_t> bloom; // bloom data: peak, base and base*
    lists_t phaselist, bridges;
    vector<int8_t> seen;         // has edge been tagged prop or bridge already
    vector<int8_t> edge_matched; // edge match status, persistent
    int phase, blooms, count_matched, pending;

    inline void add_phase(int u, int lvl) { phaselist.push(lvl, u), pending++; }
    inline void add_bridge(int e, int lvl) { bridges.push(lvl, e); }

    // find base*(u) in constant time with union-find
    inline int findstar(int u) {
        if (node[u].bloom != -1)
            u = bloom[node[u].bloom].star = findstar(bloom[node[u].bloom].star);
        return u;
    }

    inline int lowest(int e) const {
        assert(e != -1 && node[source[e]].minlevel != node[target[e]].minlevel);
        int u = source[e], v = target[e];
        return node[u].minlevel < node[v].minlevel ? u : v;
    }

    /** Reset routines
     */
    void init() {
        node.resize(V);
        phaselist.resize(V, V);
        bridges.resize(V, E);
        seen.resize(E);
        edge_matched.assign(E, false);

        count_matched = 0;
        for (int u = 0; u < V; u++) {
            if (mate[u] != -1) {
                int e = mate[u];
                assert(mate[other(e, u)] == e);
                edge_matched[e] = true, count_matched++;
            }
        }
        count_matched >>= 1; // double counted
    }

    void reset() {
        phase = blooms = ddfsid = pending = 0;
        phaselist.clear(), bridges.clear();
        for (int u = 0; u < V; u++) {
            node[u].clear();
            if (mate[u] == -1) {
                add_phase(u, 0);
                node[u].minlevel = node[u].level[0] = 0;
            }
        }
        fill(begin(seen), end(seen), 0);
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
     * For each node with min level equal to phase, find more nodes (previously unseen)
     * and find new bridges.
     *
     * - For each node u in the current phaselist
     *   - If the phase is odd, u is inner and we're looking for the matched edge out of
     *     u that goes to an unaugmented node through an unprocessed edge.
     *   - If the phase is even, u is outer and we're looking for unmatched edges out of
     *     u that go to unaugmented nodes through unprocessed edges.
     *
     *   Let the target node be v, and let the edge from u to v be e.
     *
     *   - If the min level of the node v has not been set or is phase+1 already,
     *     the edge to it is a prop and is part of the primary bfs tree. A successor and
     *     predecessor relationship is recorded for the other subroutines, and v is added
     *     to the next phaselist.
     *   - If the min level of v has been set and is <=phase then v is already in the
     *     tree and adding the edge e to it as a prop would form a back edge.
     *     Therefore e is not a prop and not part of the tree, hence it is a bridge.
     *
     *   - For bridges:
     *   - If the phase is odd then e is matched and is horizontal (i.e. the minlevel
     *     of v is phase and v is processed in the same iteration as u).
     *     The level of this bridge is known (lvl=phase) and so the bridge can be saved.
     *   - If phase is even however, the edge e is unmatched and it is possible that the
     *     evenlevel of v has not yet been computed. In this case the tenacity/level of
     *     the bridge is unknown at this point. This happens because the
     *     even level of u is smaller than the even level path to v and the bridge is
     *     not horizontal, but actually oblique in the phase graph. This bridge is
     *     ignored and will be processed later, when the even level of v is set.
     */

    void visit_prop(int e, int u, int v, bool parity) {
        assert(!seen[e] && !node[u].erased && !node[v].erased);
        if (node[v].minlevel == inf) {
            node[v].minlevel = node[v].level[!parity] = phase + 1;
            add_phase(v, phase + 1);
        }
        assert(node[v].minlevel == phase + 1 && node[v].level[!parity] == phase + 1);
        node[v].pred.push_back(u);
        node[u].succ.push_back(v);
        seen[e] = true;
    }

    void visit_bridge(int e, int parity) {
        int u = source[e], v = target[e];
        assert(!seen[e] && !node[u].erased && !node[v].erased);
        assert(node[u].level[parity] < inf && node[v].level[parity] < inf);
        int tenacity = node[u].level[parity] + node[v].level[parity] + 1;
        int lvl = tenacity >> 1;
        assert(phase <= lvl && lvl < V);
        add_bridge(e, lvl);
        seen[e] = true;
    }

    void bfs_visit(int e, int u, int v) {
        int parity = phase % 2;
        if (node[v].minlevel > phase) {
            visit_prop(e, u, v, parity);
        } else if (!seen[e] && node[v].level[parity] < inf) {
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
                for (int v : adj[u]) {
                    int e = get(u, v);
                    if (!edge_matched[e] && !seen[e] && !node[v].erased)
                        bfs_visit(e, u, v);
                }
            } else {
                int e = mate[u], v = other(e, u);
                if (!seen[e] && !node[v].erased)
                    bfs_visit(e, u, v);
            }
        }
        return false;
    }

    /** Double depth first search and MAX
     * For a given bridge 'e' with endpoints 'red' and 'blue', either find a bloom, or
     * find an augmenting path, or do nothing and exit immediately.
     *
     * The bloom formation subroutine will need to perform dfs again (this time with no
     * major fuss) to assign maxlevels, bloom ids and bloom successors and predecessors
     * to all bloom nodes.
     *
     * The augmenting path subroutine will need to reuse the trail and arc vectors.
     *
     * Nothing is done:
     *     - if red and blue were already in blooms with a common base
     *       exits trivially and immediately.
     *     - if no paths were found from red and blue to exposed vertices or to a common
     *       vertex (barrier). This should not happen at all (observation 1) and is a
     *       serious bug.
     *
     * Observation 1 (correctness optimization)
     * - A node Q is unerased iff it has not been used in an augmenting path
     *   and there is still a path from it to an exposed vertex.
     * - After augmentations, function erase_successors() erases nodes that have
     *   become unreachable from exposed vertices. Our ddfs implementation requires this
     *   procedure to be implemented properly - when advancing into a node u, there
     *   must be some path from that node to an exposed node.
     * - The predecessors of a node we're going to advance to can be stored in arbitrary
     *   order. As such, as we iterate over the predecessor array of a node u, we can
     *   eliminate nodes that have been erased by swapping them with the last element.
     *   We can also ask our implementation of erase_successors to guarantee that the
     *   first element of the predecessors array is always an unerased predecessor, to
     *   make this simpler (see Node Erasure).
     *
     * ***** Data structures
     *
     * * Three pointers: r (red, left), b (blue, right), barrier (for b).
     * * Two trail vectors, trail[0] for r and trail[1] for b.
     *   - The list of nodes leading to the root (in case of r), or to the barrier in
     *     the case of b.
     * * Two arc vectors, arc[0] for r and arc[1] for b.
     *   - Have the usual meaning. Keep track of the 'current' index into the node_pred
     *     array of a node in trail[0] or trail[1], plus r and b themselves.
     *     - The invariant len(arc[.]) = len(trail[.])+1 holds for r and b.
     *     When advancing from e.g. r, the dfs takes the path through node_pred[i]
     *     where i is the index given by the arc, and then i incremented so it always
     *     stays one index ahead of the actual trail.
     *     The position (after) of r and b is at the back of the arc.
     *
     * The "stack" of dfs nodes is the pair (trail[c],arc[c]).
     *
     * If backtracking fails for either r or b, the respective stack will become empty.
     * If backtracking failed for b the barrier is lowered to r and b is reset (but the
     * stack is not reset, which is what acts as a barrier in practice). We try
     * backtracking blue first, and if backtracking red fails too we found a bloom.
     * Otherwise we eventually find an augmenting path.
     *
     * In case we find an augmenting path we rerun dfs for blue to restore the erased
     * part of the trail.
     *
     * ***** Algorithm
     *
     * - Advance whichever of r and b is closest to red/blue (i.e. largest min_level)
     *   advancing r in case of a tie.
     *   - When advancing into nodes that belong to blooms skip directly to the
     *     bloom's base star (recursive base).
     *   - Be careful not to advance into nodes that have been erased (by a previous
     *     augmentation in the same search) (observation 2)
     *   - Notice that advancing casually (i.e. not while backtracking) only one step at
     *     a time guarantees the two paths can only cross at their endpoints (when r==b)
     *     and not somewhere in between. This is not necessarily true while backtracking
     *     either r or b as we usually need to backtrack and readvance several steps.
     *
     * - If r == b meet at the same node, this node is the dcv (deepest common vertex).
     *   - We backtrack b first. We greedily pop nodes from b's stack and readvance on
     *     their other predecessors (back-and-forth greedily) until we succeed and find
     *     a node !=r at a depth <=r, or we fail and pop the entire stack.
     *     - If this succeeds we can resume advancing.
     *   - Otherwise, b's stack is empty and there is no other path for b.
     *     - We set the barrier:=r to this node, and reset b:=barrier.
     *     - Notice that b's stack will never hold a node that is above the barrier.
     *   - We backtrack r second, the same way.
     *     - If this succeeds we can resume advancing.
     *   - Otherwise the barrier is a bottleneck and we've identified a bloom.
     *     * We return the bloom barrier.
     *
     * - If at any point both r!=b have no predecessors, i.e. they are exposed roots,
     *   we are done and found an augmenting path.
     *   * The augmenting path data are the two filled ddfs stacks.
     *   * Reconstruct the blue path's erased portion.
     */
    vector<int> trail[2], arc[2];
    int ddfsid, barrier;

    void advance_dfs(int& h, bool c) {
        int& i = arc[c].back();
        assert(uint(i) < node[h].pred.size());
        assert(node[h].vis != ddfsid || node[h].color == c);
        int hh = findstar(node[h].pred[i++]);
        assert(!node[hh].erased);

        lazy_erase_predecessors(h, i);
        node[h].vis = ddfsid;
        node[h].color = c;
        trail[c].push_back(h);
        h = hh;
        arc[c].push_back(0);
    }

    bool reverse_dfs(int& h, int c) {
        int i, s;
        if (trail[c].empty())
            return false;
        do {
            arc[c].pop_back();
            h = trail[c].back();
            trail[c].pop_back();
            i = arc[c].back();
            s = node[h].pred.size();
        } while (!trail[c].empty() && i == s);
        return i < s;
    }

    bool backtrack_dfs(int& h, int c) {
        int x = h, lvl = node[x].minlevel;
        while ((h == x || node[h].minlevel > lvl) && reverse_dfs(h, c)) {
            do {
                advance_dfs(h, c);
            } while (node[h].vis != ddfsid && node[h].minlevel > lvl);
        }
        return !trail[c].empty();
    }

    int ddfs(int peak) {
        int red = source[peak], blue = target[peak];
        int r = findstar(red), b = findstar(blue);
        if (r == b)
            return -1;

        ++ddfsid, barrier = b;
        trail[0] = {}, arc[0] = {0};
        trail[1] = {}, arc[1] = {0};

        while (node[r].minlevel != 0 || node[b].minlevel != 0) {
            if (node[r].minlevel >= node[b].minlevel) {
                advance_dfs(r, 0);
            } else {
                advance_dfs(b, 1);
            }
            if (r == b) {
                if (!backtrack_dfs(b, 1)) {
                    b = barrier = r, arc[1] = {0};
                    if (!backtrack_dfs(r, 0)) {
                        r = barrier = b, arc[0] = {0};
                        return 0;
                    }
                }
            }
        }
        assert(r != b);
        trail[0].push_back(r), trail[1].push_back(b);
        complete_blue_trail(peak);
        return 1;
    }

    void complete_blue_trail(int peak) {
        vector<int> pretrail, prearc{0};
        int u = findstar(target[peak]);
        while (u != barrier) {
            int i = prearc.back(), s = node[u].pred.size();
            while (i == s || !node[u].color) {
                assert(!pretrail.empty());
                prearc.pop_back();
                u = pretrail.back();
                pretrail.pop_back();
                i = prearc.back();
                s = node[u].pred.size();
            }
            pretrail.push_back(u);
            u = findstar(node[u].pred[prearc.back()++]);
            prearc.push_back(0);
        }
        prearc.pop_back();
        trail[1].insert(begin(trail[1]), begin(pretrail), end(pretrail));
        arc[1].insert(begin(arc[1]), begin(prearc), end(prearc));
    }

    int MAX() {
        int augmentations = 0;
        for (int peak = bridges.head[phase]; peak != -1; peak = bridges.next[peak]) {
            int red = source[peak], blue = target[peak];
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
     * All nodes reachable from the peak through predecessor edges up to the barrier have
     * been visited and properly colored. All these nodes constitute the bloom.
     * Every node u in this bloom has been colored red or blue such that:
     *   a) If u is red  there is a red  path from u to the red  peak node.
     *      If u is blue there is a blue path from u to the blue peak node.
     *   b) There is a red  path from the red  peak to the base (primary red  petal).
     *      There is a blue path from the blue peak to the base (primary blue petal).
     *   c) If u is red there  isn't necessarily a red  path from u to the base.
     *      If u is blue there isn't necessarily a blue path from u to the base.
     *
     * a) This invariant allow us to find a path from any node u to a peak using only one
     * color. If an augmenting path is found through the bloom that needs to go "around"
     * the bloom, this path is used to go from the first node to the peak, making sure
     * to use only one color as another disjoint path going down must be found too.
     *   We optimistically assume such an augmenting path will be found, so to facilitate
     * its construction, we assign bloom successors to every node alongside their
     * maxlevels. If P(u) is a path of u's color from u to the peak, then the bloom
     * successor of u is the "guide" prop edge e that leads to the second node in P(u).
     * Suppose ddfs, to reach u, went from vertex v through edge e, pointing to w.
     * Then u = base*(w) and the successor prop of u is edge e. The second node in P(u)
     * is v, before expanding blooms. Storing the edge e instead of v allows us to know
     * the vertex w immediately to expand the blooms up to u.
     *
     * b) These paths correspond to the red and blue trails constructed during ddfs. We
     * didn't store these, but we can recover them quickly with a naive dfs following
     * predecessors of the same color.
     *   If an augmenting path is found through the bloom that needs to go "around" the
     * bloom, this path is used to go down from the peak to the base, making sure to use
     * the opposite color of that used to go from the first node to the peak.
     *   To identify this "primary petal" we assign bloom predecessors to the nodes
     * on it. Only one such petal is constructed for each color.
     *
     * c) This happens because the ddfs might backtrack successfully more than one node,
     * leaving for example a blue node with only red predecessor nodes. Such nodes were
     * removed from the trail but are still part of the bloom.
     *   If an augmenting path is found through the bloom that needs to "descend" the
     * bloom directly, i.e. not go "around" it, then the colors don't matter as only a
     * single subpath through the bloom is needed, not two disjoint ones.
     *
     * Observation 1
     * - Because the bloom's nodes are found skipping nested blooms, we unfortunately
     * cannot find the primary petals and assign maxlevels/bloom ids simultaneously.
     * Therefore we must find the primary petals first and only then assign maxlevels.
     *
     * Observation 2
     * - The bloom may be one-sided, i.e. it is possible for either the red or blue
     *   trails to be empty (but not both, that would be a ddfs no-op).
     *
     * Be careful not to touch the bloom's base in any way.
     *
     * ***** Algorithm
     *
     * - Perform greedy dfs to find the primary petals. To avoid revisiting nodes that
     * lead to deadends give them a dummy pred value, -2.
     *   - Visit only pred nodes of the same color.
     *   - Once the base is found stop searching, assign preds and finish quickly.
     * - Perform dfs to assign maxlevels, bloom successors, and bloom ids.
     *   - Visit only pred nodes of the same color.
     *   - For bloom nodes with even maxlevel, look for bridges out of the bloom.
     */

    bool bloom_dfs_petal(int u) {
        if (u == bloom[blooms].base)
            return true;

        for (int v : node[u].pred) {
            int w = findstar(v);
            if (w == bloom[blooms].base) {
                node[u].bloom_pred = v;
                return true;
            }
            if (node[w].bloom_pred == -1 && node[u].color == node[w].color) {
                if (bloom_dfs_petal(w)) {
                    node[u].bloom_pred = v;
                    return true;
                }
            }
        }

        node[u].bloom_pred = -2;
        return false;
    }

    void bloom_dfs_level(int u, int p) {
        if (u == bloom[blooms].base)
            return;

        int lvl = 2 * phase + 1 - node[u].minlevel;
        assert(!node[u].erased && node[u].bloom == -1 && node[u].vis == ddfsid);
        assert(lvl < V && node[u].maxlevel == inf && node[u].level[lvl % 2] == inf);

        node[u].bloom = blooms;
        node[u].bloom_succ = p;
        node[u].maxlevel = node[u].level[lvl % 2] = lvl;
        add_phase(u, lvl);

        for (int v : node[u].pred) {
            int w = findstar(v);
            if (node[w].bloom == -1 && node[u].color == node[w].color) {
                bloom_dfs_level(w, get(u, v));
            }
        }

        if (lvl % 2 == 0) {
            for (int v : adj[u]) {
                int e = get(u, v);
                if (!node[v].erased && !seen[e] && node[v].level[0] < inf) {
                    visit_bridge(e, 0);
                }
            }
        }
    }

    void form_bloom(int peak) {
        assert(node[barrier].bloom == -1);
        int r = findstar(source[peak]), b = findstar(target[peak]);
        bloom.push_back({peak, barrier, barrier});
        bloom_dfs_petal(r);
        bloom_dfs_petal(b);
        bloom_dfs_level(r, -1);
        bloom_dfs_level(b, -1);
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
     *
     *   Unfortunately the blue trail might be partial if the barrier moved down from b,
     * in which case we just run dfs to reconstruct the missing path.
     * The red trail is instact, all the way from the red peak to the red exposed node.
     * Both trails skip nested blooms, which must be expanded. That will be the bane
     * of our existence here.
     *
     *   Each trail red/blue identifies a sequence of nodes t=u(0),u(1),u(2),...,u(n)
     * such that t is the red/blue peak and u(n) is an exposed vertex. The two sequences
     * are disjoint by construction. To construct the full path with expanded nested
     * blooms, consider the segment [u(i), u(i+1)]. There is an edge e incident on u(i)
     * such that v=other(e, u(i)) and u(i+1)=base*(v). To find out the edge e, we can
     * reuse the arc vector from ddfs; this arc is intact for the red path and is
     * reconstructed alongside the blue path.
     *
     *   If v is not in a bloom then u(i+1)=v and this is a basic segment not requiring
     * any bloom expansion. Otherwise this segment needs to be expanded with the subpath
     * from v to u(i+1). This segment may contain blooms nested both recursively (if in
     * the path from v to base(v) there are nested blooms) and iteratively (if base(v) is
     * not base*(v)=u(i+1)).
     *   When "entering" a new bloom, an orientation must be determined based on whether
     * the entering node is outer or inner. If the entering node is outer (like 3 and 4
     * in the figure) then the path through the bloom is simply "straight down" to the
     * base. However, if the entering node is inner (like 1,2,5,6 in the figure)
     * then the path is "around" the bloom and goes through the bloom's peak bridge.
     * The path going up and the path going down cannot intersect, which is why we need
     * colors to guide our walk.
     *
     * ***** Algorithm
     *
     * - For each trail red/blue with color c and top t:
     *   - Expand the initial bloom of t, if it exists.
     *   - Expand each segment [u(i),u(i+1)], i=0,...,n-1, finding the edge using arc[].
     *   - Add u(n), the exposed node.
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
        int B = node[u].bloom, peak = bloom[B].peak;

        if (node[u].minlevel % 2 == 0) /* outer */ {
            return walk_down(u, B, down);
        } else /* inner */ {
            int t = node[u].color ? source[peak] : target[peak];
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
        int peak = bloom[B].peak;
        int t = node[u].color ? target[peak] : source[peak];

        path_t top_path, path{u};
        while (node[t].bloom != B) {
            add_path(top_path, down, walk_bloom(t, down));
            t = bloom[node[t].bloom].base;
        }
        while (u != t) {
            int e = node[u].bloom_succ, v = lowest(e);
            add_path(path, !down, walk_star(v, u, down));
            u = other(e, v);
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
                u = node[u].bloom_pred; // take a predecessor of the same color
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
                u = node[u].pred.front(); // any predecessor works to go down
            } else {
                add_path(path, down, walk_bloom(u, down));
                u = bloom[node[u].bloom].base;
            }
        }
        return path;
    }

    path_t find_path(int top, int c, bool down) {
        auto path = walk_star(top, findstar(top), down);
        for (int i = 0, s = trail[c].size(); i + 1 < s; i++) {
            int u = trail[c][i], w = trail[c][i + 1];
            int j = arc[c][i] - 1, v = node[u].pred[j];
            add_path(path, down, u);
            add_path(path, down, walk_star(v, w, down));
        }
        int u = trail[c].back();
        add_path(path, down, u);
        return path;
    }

    void augment_path(int peak) {
        auto path = find_path(source[peak], 0, false);
        auto rest = find_path(target[peak], 1, true);
        path.splice(end(path), rest);

        auto ait = begin(path), bit = next(ait);
        while (bit != end(path)) {
            int u = *ait++, v = *bit++, e = get(u, v);
            assert(!node[u].erased && !node[v].erased);
            edge_matched[e] = !edge_matched[e];
            if (edge_matched[e]) {
                mate[u] = mate[v] = e;
            }
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
            for (int v : node[u].succ) {
                if (!node[v].erased && lazy_erase_predecessors(v)) {
                    node[v].erased = true;
                    path.push_back(v);
                }
            }
        }
    }

    inline bool lazy_erase_predecessors(int v, int i = 0) {
        auto& preds = node[v].pred;
        int s = preds.size();
        while (i < s && node[preds[i]].erased) {
            swap(preds[i], preds.back()), preds.pop_back(), s--;
        }
        return i == s; // erased all after i?
    }
};

#endif // GENERAL_MATCHING_HPP
