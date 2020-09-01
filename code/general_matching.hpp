#ifndef GENERAL_MATCHING_HPP
#define GENERAL_MATCHING_HPP

#include <cassert>

#include "debug_print.hpp"
#include "graph_formats.hpp"
#include "hash.hpp"

// *****

constexpr int inf = INT_MAX / 2;
constexpr int good_phase_limit = 100; // how many augmentation phases per search

/**
 * General maximum matching algorithm of Micali and Vazirani
 * Implemented from scratch, with ideas from:
 * - "Extending Search Phases in the Micali-Vazirani"
 *    Huang M., Stein C.
 *    https://github.com/mh3166/Extended_MV_algorithm
 * - "The General Maximum Matching Algorithm of Micali and Vazirani"
 *    Peterson P., Loui M.
 * Complexity: O(E V^1/2) with a monstrous constant.
 * Currently passing unit tests and random tests, compared with boost's edmonds.
 */
struct micali_vazirani {
    int V, E;
    vector<vector<int> > adj;
    vector<int> source, target, mate;
    unordered_map<pair<int, int>, int, pair_hasher> edge;

    explicit micali_vazirani(int V = 0) : V(V), E(0), adj(V), mate(V, -1) {}

    int other(int e, int u) {
        assert(u == source[e] || u == target[e]);
        return u ^ source[e] ^ target[e];
    }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        assert(!edge.count({u, v}) && !edge.count({v, u}));
        adj[u].push_back(v);
        adj[v].push_back(u);
        source.push_back(u);
        target.push_back(v);
        edge[{u, v}] = edge[{v, u}] = E++;
    }

    int max_matching() {
        int more = 1, max_matched_count = V / 2;
        init();
        while (more && count_matched < max_matched_count) {
            reset();
            more = search();
        }
        return count_matched;
    }

  private:
    vector<int> minlevel;             // node's minlevel computed by bfs
    vector<int> maxlevel;             // node's maxlevel computed by blossom ddfs
    vector<array<int, 2> > level;     // node's level per parity, for convenience
    vector<vector<int> > node_pred;   // node's bfs predecessors through prop edges
    vector<vector<int> > node_succ;   // node's bfs successors through prop edges
    vector<int> node_vis;             // ddfs flag to prevent crossing the ddfs paths
    vector<int> node_bloom_pred;      // node's bloom adjacent predecessor node, or -1
    vector<int> node_bloom_succ;      // node's bloom adjacent predecessor node, or -1
    vector<int> node_bloom;           // node's blossom, or -1
    vector<bool> node_color;          // color of a node in a petal, red(0) or blue(1)
    vector<bool> node_erased;         // whether a node is still viable
    vector<bool> edge_seen;           // whether an edge has been tagged prop or bridge
    vector<int> bloom_peak;           // peak edge of a bloom, computed by ddfs
    vector<int> bloom_base;           // base(bloom), computed from ddfs
    vector<int> bloom_star;           // base*(bloom) shortcut, disjoint-set
    vector<vector<int> > phaselist;   // bfs nodes in the frontier per phase
    vector<vector<int> > bridges;     // bridges in the frontier per phase
    vector<bool> edge_matched;        // edge match status, persistent
    int phase, blooms, count_matched; // current bfs phase, # blooms and # matches
    int num_bridges, num_pending;

    // find base*(u) in constant time with union-find
    inline int findstar(int u) {
        if (node_bloom[u] != -1)
            u = bloom_star[node_bloom[u]] = findstar(bloom_star[node_bloom[u]]);
        return u;
    }

    inline int lowest(int e) {
        assert(e != -1 && minlevel[source[e]] != minlevel[target[e]]);
        int u = source[e], v = target[e];
        return minlevel[u] < minlevel[v] ? u : v;
    }

    /** Reset routines
     */
    void init() {
        assert(mate.size() == uint(V));
        minlevel.resize(V);
        maxlevel.resize(V);
        level.resize(V);
        node_pred.resize(V);
        node_succ.resize(V);
        node_vis.resize(V);
        node_bloom_pred.resize(V);
        node_bloom_succ.resize(V);
        node_bloom.resize(V);
        node_color.resize(V);
        node_erased.resize(V);
        edge_seen.resize(E);
        phaselist.resize(V);
        bridges.resize(V);

        edge_matched.assign(E, false);
        count_matched = 0;
        for (int u = 0; u < V; u++)
            if (mate[u] != -1) {
                int e = mate[u], v = other(e, u);
                (void)v;
                assert(mate[v] == e);
                edge_matched[e] = true, count_matched++;
            }
        count_matched >>= 1; // double counted
    }

    void reset() {
        for (int u = 0; u < V; u++) {
            phaselist[u].clear(), bridges[u].clear();
        }
        for (int u = 0; u < V; u++) {
            if (mate[u] != -1) {
                minlevel[u] = level[u][0] = inf;
                maxlevel[u] = level[u][1] = inf;
            } else {
                phaselist[0].push_back(u);
                minlevel[u] = level[u][0] = 0;
                maxlevel[u] = level[u][1] = inf;
            }
            node_pred[u].clear();
            node_succ[u].clear();
            node_vis[u] = 0;
            node_bloom_pred[u] = -1;
            node_bloom_succ[u] = -1;
            node_bloom[u] = -1;
            node_color[u] = 0;
            node_erased[u] = false;
        }
        for (int e = 0; e < E; e++) {
            edge_seen[e] = false;
        }
        bloom_peak.clear();
        bloom_base.clear();
        bloom_star.clear();
        phase = blooms = ddfsid = num_bridges = 0;
        num_pending = phaselist[0].size();
    }

    int search() {
        bool done = false;
        int more, augmentations = 0, good = 0;
        while (!done && good < good_phase_limit && count_matched < V / 2) {
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
        assert(!edge_seen[e] && !node_erased[u] && !node_erased[v]);
        if (minlevel[v] == inf) {
            minlevel[v] = level[v][!parity] = phase + 1;
            phaselist[phase + 1].push_back(v), num_pending++;
        }
        assert(minlevel[v] == phase + 1 && level[v][!parity] == phase + 1);
        node_pred[v].push_back(u);
        node_succ[u].push_back(v);
        edge_seen[e] = true;
    }

    void visit_bridge(int e, int parity) {
        int u = source[e], v = target[e];
        assert(!edge_seen[e] && !node_erased[u] && !node_erased[v]);
        assert(level[u][parity] < inf && level[v][parity] < inf);
        int tenacity = level[u][parity] + level[v][parity] + 1;
        int lvl = tenacity >> 1;
        assert(phase <= lvl && lvl < V);
        bridges[lvl].push_back(e), num_bridges++;
        edge_seen[e] = true;
    }

    void bfs_visit(int e, int u, int v) {
        int parity = phase % 2;
        if (minlevel[v] > phase) {
            visit_prop(e, u, v, parity);
        } else if (!edge_seen[e] && level[v][parity] < inf) {
            visit_bridge(e, parity);
        }
    }

    bool MIN() {
        if (num_pending == 0) {
            return true;
        }
        bool parity = phase % 2;
        for (int u : phaselist[phase]) {
            if (node_erased[u])
                continue;
            if (parity == 0) {
                for (int v : adj[u]) {
                    int e = edge.at({u, v});
                    if (!edge_matched[e] && !edge_seen[e] && !node_erased[v])
                        bfs_visit(e, u, v);
                }
            } else {
                int e = mate[u], v = other(e, u);
                if (!edge_seen[e] && !node_erased[v])
                    bfs_visit(e, u, v);
            }
        }
        num_pending -= phaselist[phase].size();
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
        assert(uint(i) < node_pred[h].size());
        assert(node_vis[h] != ddfsid || node_color[h] == c);
        int hh = findstar(node_pred[h][i++]);
        assert(!node_erased[hh]);

        lazy_erase_predecessors(h, i);
        node_vis[h] = ddfsid;
        node_color[h] = c;
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
            s = node_pred[h].size();
        } while (!trail[c].empty() && i == s);
        return i < s;
    }

    bool backtrack_dfs(int& h, int c) {
        int x = h, lvl = minlevel[x];
        while ((h == x || minlevel[h] > lvl) && reverse_dfs(h, c)) {
            do {
                advance_dfs(h, c);
            } while (node_vis[h] != ddfsid && minlevel[h] > lvl);
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

        while (minlevel[r] != 0 || minlevel[b] != 0) {
            if (minlevel[r] >= minlevel[b]) {
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
            int i = prearc.back(), s = node_pred[u].size();
            while (i == s || !node_color[u]) {
                assert(!pretrail.empty());
                prearc.pop_back();
                u = pretrail.back();
                pretrail.pop_back();
                i = prearc.back();
                s = node_pred[u].size();
            }
            pretrail.push_back(u);
            u = findstar(node_pred[u][prearc.back()++]);
            prearc.push_back(0);
        }
        prearc.pop_back();
        trail[1].insert(begin(trail[1]), begin(pretrail), end(pretrail));
        arc[1].insert(begin(arc[1]), begin(prearc), end(prearc));
    }

    int MAX() {
        int augmentations = 0;
        for (uint i = 0; i < bridges[phase].size(); i++) {
            int peak = bridges[phase][i];
            int red = source[peak], blue = target[peak];
            if (node_erased[red] || node_erased[blue]) {
                continue;
            }
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
        if (u == bloom_base[blooms])
            return true;
        if (node_bloom_pred[u] == -2)
            return false;

        for (int v : node_pred[u]) {
            int w = findstar(v);
            if (w == bloom_base[blooms] || node_color[u] == node_color[w]) {
                if (bloom_dfs_petal(w)) {
                    node_bloom_pred[u] = v;
                    return true;
                }
            }
        }

        node_bloom_pred[u] = -2;
        return false;
    }

    void bloom_dfs_level(int u, int p) {
        if (u == bloom_base[blooms] || node_bloom[u] == blooms)
            return;

        int lvl = 2 * phase + 1 - minlevel[u];
        assert(!node_erased[u]);
        assert(node_bloom[u] == -1);
        assert(node_vis[u] == ddfsid);
        assert(lvl < V);
        assert(maxlevel[u] == inf);
        assert(level[u][lvl % 2] == inf);

        node_bloom[u] = blooms;
        node_bloom_succ[u] = p;
        maxlevel[u] = level[u][lvl % 2] = lvl;
        phaselist[lvl].push_back(u), num_pending++;

        for (int v : node_pred[u]) {
            int w = findstar(v), e = edge.at({u, v});
            if (node_color[u] == node_color[w]) {
                bloom_dfs_level(w, e);
            }
        }

        if (lvl % 2 == 0) {
            for (int v : adj[u]) {
                int e = edge.at({u, v});
                if (!node_erased[v] && !edge_seen[e] && level[v][0] < inf) {
                    visit_bridge(e, 0);
                }
            }
        }
    }

    void form_bloom(int peak) {
        assert(node_bloom[barrier] == -1);
        int r = findstar(source[peak]), b = findstar(target[peak]);
        bloom_peak.push_back(peak);
        bloom_base.push_back(barrier);
        bloom_star.push_back(barrier);
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
     * - Reconstruct the missing blue path using dfs
     *   - Like ddfs, use a similar advance/reverse combo
     *   - TODO: can this reconstruction be avoided at little expense?
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

    path_t walk_bloom(int u, bool down) {
        int B = node_bloom[u], peak = bloom_peak[B];

        if (minlevel[u] % 2 == 0) /* outer */ {
            return walk_down(u, B, down);
        } else /* inner */ {
            int t = node_color[u] ? source[peak] : target[peak];
            auto to_peak = walk_peak(u, B, !down);
            auto to_base = walk_base(t, B, down);
            auto where = down ? end(to_peak) : begin(to_peak);
            to_peak.splice(where, to_base);
            return to_peak;
        }
    }

    path_t walk_star(int u, int star, bool down) {
        path_t path;
        while (u != star) {
            auto subpath = walk_bloom(u, down);
            auto where = down ? end(path) : begin(path);
            path.splice(where, subpath);
            u = bloom_base[node_bloom[u]];
        }
        return path;
    }

    path_t walk_peak(int u, int B, bool down) {
        int peak = bloom_peak[B];
        int t = node_color[u] ? target[peak] : source[peak];

        path_t top_path, path{u};
        while (node_bloom[t] != B) {
            auto subpath = walk_bloom(t, down);
            auto where = down ? end(top_path) : begin(top_path);
            top_path.splice(where, subpath);
            t = bloom_base[node_bloom[t]];
        }
        while (u != t) {
            int e = node_bloom_succ[u], v = lowest(e);
            auto subpath = walk_star(v, u, down); // usually empty
            auto where = down ? begin(path) : end(path);
            path.splice(where, subpath);
            u = other(e, v);
            down ? path.push_front(u) : path.push_back(u);
        }
        auto where = down ? begin(path) : end(path);
        path.splice(where, top_path);
        return path;
    }

    path_t walk_base(int u, int B, bool down) {
        int base = bloom_base[B];

        path_t path;
        while (u != base) {
            if (node_bloom[u] == B) {
                down ? path.push_back(u) : path.push_front(u);
                u = node_bloom_pred[u]; // take a predecessor of the same color
            } else {
                auto subpath = walk_bloom(u, down);
                auto where = down ? end(path) : begin(path);
                path.splice(where, subpath);
                u = bloom_base[node_bloom[u]];
            }
        }
        return path;
    }

    path_t walk_down(int u, int B, bool down) {
        int base = bloom_base[B];

        path_t path;
        while (u != base) {
            if (node_bloom[u] == B) {
                down ? path.push_back(u) : path.push_front(u);
                u = node_pred[u].front(); // any predecessor works to go down
            } else {
                auto subpath = walk_bloom(u, down);
                auto where = down ? end(path) : begin(path);
                path.splice(where, subpath);
                u = bloom_base[node_bloom[u]];
            }
        }
        return path;
    }

    path_t find_path(int top, int c, bool down) {
        auto path = walk_star(top, findstar(top), down);
        for (int i = 0, s = trail[c].size(); i + 1 < s; i++) {
            int u = trail[c][i], w = trail[c][i + 1];
            int j = arc[c][i] - 1, v = node_pred[u][j];
            down ? path.push_back(u) : path.push_front(u);
            auto rest = walk_star(v, w, down);
            auto where = down ? end(path) : begin(path);
            path.splice(where, rest);
        }
        int u = trail[c].back();
        down ? path.push_back(u) : path.push_front(u);
        return path;
    }

    path_t construct_augmenting_path(int peak) {
        auto path = find_path(source[peak], 0, false);
        auto rest = find_path(target[peak], 1, true);
        path.splice(end(path), rest);
        return path;
    }

    void augment_path(int peak) {
        auto path = construct_augmenting_path(peak);

        bool status = 0;
        auto ait = begin(path), bit = next(ait);
        while (bit != end(path)) {
            int u = *ait++, v = *bit++, e = edge.at({u, v});
            assert(!node_erased[u] && !node_erased[v] && status == edge_matched[e]);
            status = !status;
            edge_matched[e] = !edge_matched[e];
            if (edge_matched[e]) {
                mate[u] = mate[v] = e;
            }
        }
        for (int u : path) {
            node_erased[u] = true;
        }
        erase_successors(path);
    }

    void erase_successors(path_t& path) {
        while (!path.empty()) {
            int u = path.front();
            path.pop_front();
            for (int v : node_succ[u]) {
                if (!node_erased[v] && lazy_erase_predecessors(v)) {
                    node_erased[v] = true;
                    path.push_back(v);
                }
            }
        }
    }

    inline bool lazy_erase_predecessors(int v, int i = 0) {
        auto& preds = node_pred[v];
        int s = preds.size();
        while (i < s && node_erased[preds[i]]) {
            swap(preds[i], preds.back()), preds.pop_back(), s--;
        }
        return i == s; // erased all after i?
    }
};

#endif // GENERAL_MATCHING_HPP
