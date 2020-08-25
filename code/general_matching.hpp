#include "debug_print.hpp"
#include "hash.hpp"

// *****

void print_edges(const vector<int>& source, const vector<int>& target) {
    int E = source.size();
    print(stderr, "{:>{}}    ", "source", PAD - 2);
    for (int e = 0; e < E; e++) {
        print(stderr, "{:>{}} ", source[e], dw);
    }
    print(stderr, "\n");
    print(stderr, "{:>{}}    ", "target", PAD - 2);
    for (int e = 0; e < E; e++) {
        print(stderr, "{:>{}} ", target[e], dw);
    }
    print(stderr, "\n");
}

constexpr int inf = INT_MAX / 2;
constexpr int good_phase_limit = 100; // how many augmentation phases per search

/**
 * General maximum matching algorithm of Micali and Vazirani
 * Implemented from scratch, with ideas from:
 * - "Extending Search Phases in the Micali-Vazirani", Huang M., Stein C.
 *           https://github.com/mh3166/Extended_MV_algorithm
 * - "The General Maximum Matching Algorithm of Micali and Vazirani", Peterson P, Loui M.
 *
 * Complexity: O(E V^1/2) with a monstrous constant.
 */
struct micali_vazirani {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source, target, mate;
    unordered_map<pair<int, int>, int, pair_hasher> edge;

    explicit micali_vazirani(int V = 0) : V(V), E(0), adj(V), mate(V, -1) {}

    int other(int e, int u) {
        return u == target[e] ? source[e] : (assert(u == source[e]), target[e]);
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
    vector<int> minlevel;               // node's minlevel computed by bfs
    vector<int> maxlevel;               // node's maxlevel computed by blossom ddfs
    vector<array<int, 2>> level;        // node's level per parity, for convenience
    vector<vector<int>> node_pred;      // node's bfs predecessors through prop edges
    vector<vector<int>> node_succ;      // node's bfs successors through prop edges
    vector<int> node_vis;               // ddfs flag to prevent crossing the ddfs paths
    vector<int> node_bloom_pred;        // node's petal adjacent predecessor node, or -1
    vector<int> node_bloom;             // node's blossom, or -1
    vector<bool> node_color;            // color of a node in a petal, red(0) or blue(1)
    vector<bool> node_erased;           // whether a node is still viable
    vector<bool> edge_seen;             // whether an edge has been visited by bfs
    vector<int> bloom_peak;             // peak edge of a bloom, computed by ddfs
    vector<int> bloom_base;             // base(bloom), computed from ddfs
    vector<int> bloom_star;             // base*(bloom) shortcut, disjoint-set
    vector<vector<int>> phaselist;      // bfs nodes in the frontier per phase
    vector<vector<int>> bridges;        // bridges in the frontier per phase
    vector<bool> edge_matched;          // edge match status, persistent
    int phase, blossoms, count_matched; // current bfs phase, # blossoms and # matches

    // find base*(u) in constant time with union-find
    inline int findstar(int u) {
        if (node_bloom[u] != -1)
            u = bloom_star[node_bloom[u]] = findstar(bloom_star[node_bloom[u]]);
        return u;
    }

    /** Reset routines
     */
    void init() {
        mate.resize(V, -1);
        minlevel.resize(V);
        maxlevel.resize(V);
        level.resize(V);
        node_pred.resize(V);
        node_succ.resize(V);
        node_vis.resize(V);
        node_bloom_pred.resize(V);
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
        phase = blossoms = ddfsid = 0;
    }

    /** Breadth first search and MIN
     * For each node with min level equal to phase, find more nodes and find bridges.
     *
     * - For each node u in this phaselist
     *   - If the phase is odd, u is inner and we're looking for the matched edge out of
     *     u that go to an unaugmented nodes through unprocessed edges.
     *   - If the phase is even, u is outer and we're looking for unmatched edges out of
     *     u that go to unaugmented nodes through unprocessed edges.
     *
     *   - If the min level of the target node v has not been set or is phase+1 already,
     *     the edge to it is a prop and is part of the primary bfs tree. A successor and
     *     predecessor relationship is recorded for the other subroutines, and v is added
     *     to the next phaselist.
     *   - If the min level of v has been set and is <=phase then v is already in the
     *     tree and adding the edge e to v would form a non-vertical cycle.
     *     Therefore e is not a prop and not part of the tree,
     *     hence it is a bridge (read: back edge).
     *
     *   - For prop edges:
     *   - If the phase is odd, the edge is matched and is horizontal (i.e. the minlevel
     *     of v is phase and v is processed in the same iteration). The tenacity of this
     *     bridge is known and so the bridge can be saved.
     *   - If phase is even however, the edge is unmatched and it is possible that the
     *     evenlevel of the other node has not yet been computed at all. In this case
     *     the tenacity of the bridge is unknown at this point. This happens because the
     *     even_level of u is smaller than the even_level path to v and the bridge is
     *     not horizontal, but actually oblique in the phase graph. This bridge is
     *     ignored and will be processed later, when the even level of v is set.
     */

    void visit_prop(int e, int u, int v, bool parity) {
        assert(!edge_seen[e] && other(e, u) == v);
        if (minlevel[v] == inf) {
            minlevel[v] = level[v][!parity] = phase + 1;
            phaselist[phase + 1].push_back(v);
        }
        assert(minlevel[v] == phase + 1 && level[v][!parity] == phase + 1);
        node_pred[v].push_back(u);
        node_succ[u].push_back(v);
        edge_seen[e] = true;
    }

    void visit_bridge(int e, int parity) {
        int u = source[e], v = target[e];
        assert(!edge_seen[e]);
        assert(level[u][parity] < inf && level[v][parity] < inf);
        int tenacity = level[u][parity] + level[v][parity] + 1;
        int lvl = tenacity >> 1;
        assert(lvl >= phase);
        bridges[lvl].push_back(e);
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
        if (phaselist[phase].empty()) {
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
                assert(edge_matched[e]);
                if (!edge_seen[e] && !node_erased[v])
                    bfs_visit(e, u, v);
            }
        }
        phaselist[phase].clear();
        return false;
    }

    /** Double depth first search and MAX
     * For a given bridge 'e' with endpoints 'red' and 'blue', either find a blossom, or
     * find an augmenting path, or do nothing.
     *
     * If a blossom is found we identify:
     *     - the peak (e)
     *     - the base (r=b=barrier, the bottleneck)
     *     - the red support, with embedded blossoms properly skipped
     *     - the blue support, with embedded blossoms properly skipped
     *
     * If an augmenting path is found we identify:
     *     - the two endpoints x=r and y=b of the augmenting path.
     *     - a contracted path from red to x (the red path), with blossoms skipped
     *     - a contracted path from blue to y (the blue path), with blossoms skipped
     *     - the path is a valid augmenting path with 2*phase+1 edges.
     *
     * Nothing is done:
     *     - if red and blue were already in blossoms with a common base
     *       exits trivially and immediately.
     *     - if no paths were found from red and blue to exposed vertices or to a common
     *       vertex (barrier). This should not happen at all (observation 1) and is a
     *       serious bug.
     *
     * Observation 1 (correctness)
     * - A node Q is unerased iff it has not been used in an augmenting path
     *   and there is still a path from it to an exposed vertex.
     * - After augmentations, function erase_successors() erases nodes that have
     *   become unreachable from exposed vertices. Our ddfs implementation requires this
     *   procedure to be implemented properly - when advancing into a node u, there
     *   must be some path from that node to an exposed node.
     *
     * Observation 2 (optimization)
     * - The predecessors of a node we're going to advance to can be stored in arbitrary
     *   order. As such, as we iterate over the predecessor array of a node u, we can
     *   eliminate nodes that have been erased by swapping them with the last element.
     *   We can also ask our implementation of erase_successors to guarantee that the
     *   first element of the predecessors array is always an unerased predecessor, to
     *   make this simpler.
     * - This optimization is actually required to guarantee linear complexity.
     *   The alternative would be to use unordered_set for node_pred and erase preds
     *   in erase_successors.
     *
     * ***** Data structures
     *
     * * Two pointers: r (red, head[0], left) and b (blue, head[1], right)
     * * Two supports, support[r] and support[b]
     *   - Hold the paths leading from the origins red and blue, not including r or b.
     * * Two saved supports, saved[r] and saved[b]
     *   - Used when backtracking either r and b fails and the support, which was
     *     automatically corrected, needs to be reset to its original value.
     * * Two arc vectors, arc[r] and arc[b].
     *   - Have the usual meaning. Keep track of the 'current' index into the node_pred
     *     array of a node in trail[r] or trail[b], plus r and b themselves.
     *     - The invariant len(arc[.]) = len(trail[.])+1 holds for r and b.
     *     When advancing from e.g. r, the dfs takes the path through node_pred[i]
     *     where i is the index given by the arc, and then i incremented so it always
     *     stays one index ahead of the actual trail.
     *     The position (after) of the r and b is at the back of the arc.
     *   - Little trick: the ddfs does not need the arc vectors to be saved, but we'll
     *     do so to speed up the following step (augmenting or forming a blossom) which
     *     will prevent us from travelling through the node_pred array all over again.
     * * Two trail vectors, trail[r] and trail[b], called the stacks.
     *   - The list of nodes leading to the root (in case of r), or to the barrier in
     *     the case of b.
     *
     * ***** Algorithm
     *
     * - Advance whichever of r and b is closest to red/blue (i.e. largest min_level)
     *   advancing r in case of a tie.
     *   - When advancing into nodes that belong to blossoms skip directly to the
     *     blossom's base star (recursive base).
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
     *     - If this succeeds we can set saved[b]:=support[b] and resume advancing.
     *   - Otherwise, b's stack is empty and there is no other path for b.
     *     - We set the barrier:=r to this node.
     *     - Notice that b's stack will never hold a node that is above the barrier.
     *     - We reset b's support: support[b]:=saved[b]
     *   - We backtrack r second, the same way.
     *     - If this succeeds we can set saved[r]:=support[r] and resume advancing.
     *   - Otherwise the barrier is a bottleneck and we've identified a blossom.
     *     - We reset r's support: support[r]:=saved[r]
     *     * We return the blossom data: barrier, support[r] and support[b].
     *
     * - If at any point both r!=b have no predecessors, i.e. they are exposed roots,
     *   we are done and found an augmenting path.
     *   * The augmenting path data is: support[r] and support[b], with r and b added.
     */
    vector<int> trail[2], saved[2], support[2], taken[2], arc[2];
    int ddfsid;

    void advance_dfs(int& h, int c) {
        int& i = arc[c].back();
        int hh = findstar(node_pred[h][i++]);
        assert(uint(i) <= node_pred[h].size() && !node_erased[hh]);
        lazy_erase_predecessors(h, i);
        node_vis[h] = ddfsid;
        support[c].push_back(h);
        trail[c].push_back(h);
        arc[c].push_back(0);
        h = hh;
    }

    bool reverse_dfs(int& h, int c) {
        int i, s;
        assert(!trail[c].empty());
        do {
            h = trail[c].back();
            arc[c].pop_back();
            trail[c].pop_back();
            support[c].pop_back();
            node_vis[h] = 0;
            i = arc[c].back();
            s = node_pred[h].size();
        } while (!trail[c].empty() && i == s);
        return !trail[c].empty() || i < s;
    }

    bool backtrack_dfs(int& h, int c) {
        int x = h, lvl = minlevel[x];
        while ((h == x || node_vis[h] == ddfsid) && reverse_dfs(h, c)) {
            do {
                advance_dfs(h, c);
            } while (node_vis[h] != ddfsid && minlevel[h] > lvl);
        }
        return minlevel[h] <= lvl && h != x;
    }

    int ddfs(int e) {
        int red = source[e], blue = target[e];
        int r = findstar(red), b = findstar(blue), barrier = -1;
        if (r == b)
            return -2;

        dprintin("@ddfs e={} red={} blue={} r={} b={}\n", e, red, blue, r, b);
        node_vis[r] = node_vis[b] = ++ddfsid;
        trail[0] = {}, taken[0] = arc[0] = {0}, saved[0] = support[0] = {};
        trail[1] = {}, taken[1] = arc[1] = {0}, saved[1] = support[1] = {};

        while (minlevel[r] != 0 || minlevel[b] != 0) {
            dprintin("@ddfs r={} b={}  sr={}  sb={}\n", r, b, support[0], support[1]);
            dprint("ddfs  t0={}  t1={}\n", taken[0], taken[1]);
            dprint("ddfs  a0={}  a1={}\n", arc[0], arc[1]);
            dprint("ddfs  s0={}  s1={}\n", saved[0], saved[1]);
            if (minlevel[r] >= minlevel[b]) {
                advance_dfs(r, 0);
                saved[0] = support[0]; // TODO
                taken[0] = arc[0];     // TODO
            } else {
                advance_dfs(b, 1);
                saved[1] = support[1]; // TODO
                taken[1] = arc[1];     // TODO
            }
            if (r == b) {
                if (backtrack_dfs(b, 1)) {
                    saved[1] = support[1]; // TODO
                    taken[1] = arc[1];     // TODO
                } else {
                    support[1] = saved[1];             // TODO
                    arc[1] = {0}, taken[1].back() = 0; // TODO
                    b = barrier = r;
                    if (backtrack_dfs(r, 0)) {
                        saved[0] = support[0]; // TODO
                        taken[0] = arc[0];     // TODO
                    } else {
                        support[0] = saved[0]; // TODO
                        r = barrier = b;
                        // found blossom, output: barrier, support[] and taken[]
                        dprint("~ barrier={}\n", barrier);
                        dprint("sr={}  sb={}\n", support[0], support[1]);
                        dprintout("t0={}  t1={}\n", taken[0], taken[1]);
                        dprintout("@ddfs === blossom\n");
                        return barrier;
                    }
                }
            }
            dprintout("@ddfs ===\n");
        }
        assert(r != b); // this would be a blossom, needs to be caught inside the loop

        // found augmenting path, output: support[], taken[]
        support[0].push_back(r), support[1].push_back(b);
        dprint("~ barrier={}\n", barrier);
        dprint("sr={}  sb={}\n", support[0], support[1]);
        dprint("t0={}  t1={}\n", taken[0], taken[1]);
        dprintout("@ddfs === augmenting path\n");
        return -1;
    }

    int MAX() {
        int augmentations = 0;
        for (int e : bridges[phase]) {
            int red = source[e], blue = target[e];
            if (node_erased[red] || node_erased[blue]) {
                continue;
            }
            int found = ddfs(e);
            if (found == -1) {
                augment_path(e);
                augmentations++;
            } else if (found >= 0) {
                form_blossom(e, found);
                blossoms++;
            }
        }
        return augmentations;
    }

    /** Blossom formation and coloring
     *
     * After ddfs we have:
     *   The red and blue supports (aka petals) of the blossom (support[])
     *   The predecessors used to form the petals (taken[])
     *
     * By the invariants of the ddfs, each node in the support has no blossom.
     * Afterwards, give each support node its max level as computed from the tenacity.
     *
     * ***** Algorithm
     *
     * - For each petal (red and blue)
     *   - For each node u in the support, in order (from peak to base) assign its bloom,
     *     color, and max level. Assign it also its bloom predecessor that was computed
     *     by the ddfs - this predecessor is usually the next node in the support, but
     *     if a blossom was skipped then its a node in the skipped blossom, usually one
     *     of the peak nodes.
     *   - If the node is inner (its maxlevel just assigned is even) then search for
     *     bridges emanating from it, just like we did in the visit_bfs subroutine.
     */

    void form_petal(int B, int c) {
        const auto& sup = support[c];
        for (int i = 0, s = sup.size(); i < s; i++) {
            int u = sup[i];
            assert(node_bloom[u] == -1);
            node_bloom[u] = B;
            node_color[u] = c;

            int lvl = 2 * phase + 1 - minlevel[u];
            assert(lvl < V && maxlevel[u] == inf && level[u][lvl % 2] == inf);

            maxlevel[u] = level[u][lvl % 2] = lvl;
            phaselist[lvl].push_back(u);
            if (lvl % 2 == 0) {
                for (int v : adj[u]) {
                    int e = edge.at({u, v});
                    if (!edge_seen[e] && level[v][0] < inf) {
                        visit_bridge(e, 0);
                    }
                }
            }

            assert(taken[c][i] >= 1);
            int j = taken[c][i] - 1, v = node_pred[sup[i]][j];
            node_bloom_pred[sup[i]] = v;
        }
        dheader("nodes", V);
        debug(node_bloom_pred);
        dprint("sup.back()={}  B={}  base={}\n", sup.back(), B, bloom_base[B]);
        assert(node_bloom_pred[sup.back()] == bloom_base[B]);
    }

    void form_blossom(int peak, int base) {
        dprintin("@form_blossom peak={} base={}\n", peak, base);
        int B = blossoms++;
        bloom_peak.push_back(peak);
        bloom_base.push_back(base);
        bloom_star.push_back(findstar(base));
        form_petal(B, 0);
        form_petal(B, 1);
        dprintout("@form_blossom B={}\n", B);
    }

    /** Path augmentation and blossom expansion
     *
     * Observation 1 (algorithm)
     * - We find the augmenting path that goes like this:
     *   back(support[0]) ... front(support[0]) front(support[1]) ... back(support[1])
     *      ^ final r             ^ red             ^ blue               ^ final b
     * - Expect that 'red' and 'blue' are actually the ddfs's initial r and b in case
     *   red and blue were in blossoms, so we account for that as well.
     *
     * Observation 2 (optimization)
     * - This is a good one. We can reuse the taken vectors from the ddfs subroutine to
     *   figure out the edge that leads us from support[i] to support[i + 1]. From there
     *   we can follow petal pred edges all the way to lo, the bloom star, by carefully
     *   turning around in nested blossoms if necessary.
     *
     * ***** Algorithm
     *
     * - Compute two paths for each support (red and blue) as follows:
     * - Expand initially subpath [red, r) or [blue, b) and then each subpath
     *   [support[i], support[i + 1]) to build the whole path.
     *   At each point, add the head [i] but not the tail [i+1].
     *   - This subpath is empty in the first case when red=r or blue=b.
     *     - Otherwise it is not empty as it includes the head, at least.
     *   - This segment is usually singular, i.e. the nodes are adjacent and the
     *     expansion has exactly one node, [i].
     *   - The segment may be a star jump, in which case finding the path is non-trivial
     *     and requires carefully walking through the peaks and petals of the nested
     *     blossoms. In this case, we check taken[i] to find the predecessor of [i] that
     *     the ddfs selected, that is in a blossom whose star is [i+1].
     * - Finally, append the exposed vertex to the path (support.back())
     *
     * ***** Expansion of blossoms
     *
     * - To compute the path down along a bloom B from any node u in B to another node v
     *   that is in B or is B's base, we use 'walk_blossom'. This is the primary
     *   path construction routine, which goes through the petal predecessor pointers,
     *   detecting arbitrarily nested blooms and recursing into them.
     * - To compute the actual correct path from a node u in B to B's base, we use
     *   'orient_blossom', which calls walk_blossom once or twice depending on whether u
     * is an inner or outer node in B.
     * - To compute each segment is pretty simple: the path from sup[i] to sup[i + 1]
     *   is hi plus the possibly empty path from the ddfs's predecessor of hi to
     *   sup[i + 1]. The later is either empty or only goes through nodes that like in
     *   blossoms.
     * - To construct the whole thing from the support we use 'find_path'.
     *
     * ***** Node erasure
     *
     * - Set the erased flag for each node in the augmenting path. For each such node,
     *   go to its successor and decrease that successor's count of viable predecessors.
     *   If this number reached 0, then that successor has no remaining paths to an
     *   exposed vertex and therefore can be forgotten and marked as erased as well.
     * - Our implementation of ddfs asks for a favor: although we do not always remove
     *   the erased nodes from their successors' predecessor list, we must guarantee the
     *   first node (at index 0) of each predecessor list remains unerased. Then the
     *   ddfs advance subroutine will take care of the rest.
     */

    int orient_blossom(list<int>& path, int u, bool down = true) {
        int B = node_bloom[u], base = bloom_base[B], peak = bloom_peak[B];
        int red = source[peak], blue = target[peak];

        if (minlevel[u] % 2 == 0) /* outer */ {
            walk_blossom(path, u, base, B, down);
        } else if (node_color[u] == 0) /* red inner */ {
            path.push_back(u);
            walk_blossom(path, red, u, B, !down);
            walk_blossom(path, blue, base, B, down);
        } else /* blue inner */ {
            path.push_back(u);
            walk_blossom(path, blue, u, B, !down);
            walk_blossom(path, red, base, B, down);
        }
        return base;
    }

    void walk_blossom(list<int>& path, int hi, int lo, int B, bool down) {
        assert(node_bloom[lo] == B || lo == bloom_base[B]);
        list<int> subpath;
        while (hi != lo) {
            if (node_bloom[hi] == B) {
                down ? subpath.push_back(hi) : subpath.push_front(hi);
                hi = node_bloom_pred[hi];
            } else {
                hi = orient_blossom(subpath, hi, down);
            }
        }
        path.splice(end(path), subpath);
    }

    void walk_star(list<int>& path, int hi, int lo) {
        assert(lo == findstar(hi));
        while (hi != lo) {
            hi = orient_blossom(path, hi);
        }
    }

    list<int> find_path(int hi, int c) {
        list<int> path;
        walk_star(path, hi, support[c][0]);
        for (int i = 0, s = support[c].size(); i + 1 < s; i++) {
            int v = node_pred[support[c][i]][taken[c][i] - 1];
            path.push_back(support[c][i]);
            walk_star(path, v, support[c][i + 1]);
        }
        path.push_back(support[c].back());
        return path;
    }

    void augment_path(int peak) {
        dprintin("@augment_path: peak={} rs={} bs={}\n", peak, support[0], support[1]);
        auto path = find_path(source[peak], 0);
        reverse(begin(path), end(path));
        path.splice(end(path), find_path(target[peak], 1));

        dprint("augmenting path: {}\n", path);
        assert(path.size() == 2U * phase + 2);

        bool status = 0;
        auto ait = begin(path), bit = next(ait);
        while (bit != end(path)) {
            int u = *ait++, v = *bit++, e = edge.at({u, v});
            assert(!node_erased[u] && !node_erased[v] && status == edge_matched[e]);
            status = !status;
            edge_matched[e] = !edge_matched[e];
            if (edge_matched[e])
                mate[u] = mate[v] = e;
        }
        for (int u : path) {
            node_erased[u] = true;
        }
        erase_successors(path);
        dprintout("@augment_path out peak={}\n", peak);
    }

    void erase_successors(list<int>& path) {
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

    bool lazy_erase_predecessors(int v, int i = 0) {
        auto& preds = node_pred[v];
        int s = preds.size();
        while (i < s && node_erased[preds[i]]) {
            swap(preds[i], preds.back()), preds.pop_back(), s--;
        }
        return i == s; // erased all after i?
    }

    /** Search routine
     * Using the optimizations shown in this paper:
     * https://pdfs.semanticscholar.org/e6ca/ff814bba9949fce6a48c76e3b158a8ddafbb.pdf
     */
    int search() {
        bool done = false;
        int more, augmentations = 0, good = 0;
        while (!done && phase < V && good < good_phase_limit && count_matched < V / 2) {
            done = MIN();
            more = MAX();
            good += more > 0;
            count_matched += more;
            augmentations += more;
            phase++;
        }
        return augmentations;
    }
};
