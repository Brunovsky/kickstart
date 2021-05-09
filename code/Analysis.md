# Implementation notes

## General matching: Micali-Vazirani

In `matching/micali_vazirani.hpp` we solve matching on non-bipartite graphs with MV.

Heavily optimized:

- `findpath` does not do depth first search
    - but it still constructs `list<int>` which could be improved
- no hashtables used
- no data structure required to hold a bloom's support
- efficient storage of phaselists and bridges
- two very good greedy maximal matching algorithms that run in O(V + E) time
    - node_bootstrap() generally yields a better maximal matching
    - edge_bootstrap() is simpler and also good
- extended search phases supported, tunable

### Breadth first search and MIN

For each node with min level equal to phase, find more nodes (previously unseen) and find
new bridges.

For each node u in the current phaselist:

- If the phase is odd, u is inner and we're looking for the matched edge out of u that
    goes to an unaugmented node through an unprocessed edge.
- If the phase is even, u is outer and we're looking for unmatched edges out of u that go
    to unaugmented nodes through unprocessed edges.

Let the target node be v, and let the edge from u to v be e.

- If the minlevel of the node v has not been set or is phase+1 already, the edge to it is
    a prop and is part of the primary bfs tree. A successor and predecessor relationship
    is recorded for the other subroutines, and v is added to the next phaselist.
- If the minlevel of v has been set and is <=phase then v is already in the tree and
    adding the edge e to it as a prop would form a back edge. Therefore e is not a prop
    and not part of the tree, hence it is a bridge.

For bridges:

- If the phase is odd then e is matched and is horizontal (i.e. the minlevel of v is phase
    and v is processed in the same iteration as u). The level of this bridge is known
    (lvl=phase) and so the bridge can be saved.
- If phase is even however, the edge e is unmatched and it is possible that the evenlevel
    of v has not yet been computed. In this case the tenacity/level of the bridge is
    unknown at this point. This happens because the even level of u is smaller than the
    even level path to v and the bridge is not horizontal, but actually oblique in the
    phase graph. This bridge is ignored and will be processed later, when the even level
    of v is set.

### Double depth first search and MAX

For a given bridge `e` with endpoints 'red' and 'blue', either find a bloom, or find an
augmenting path, or do nothing and exit immediately.

The idea is straightforward: we maintain two dfs "trails" instead of just one. The goal is
to arrive at two distinct exposed nodes and thus identify an augmenting path made up of
the concatenation of both trails, but this will be impossible in the presence of a bloom.

The two trails are colored red and blue, and along the ddfs the nodes visited are
appropriately colored. Predecessor relationships are stored, which maintain the trail data
attached to the nodes.

The trails cannot enter into nodes visited by the other trail, identified by their color,
even if the other trail has meanwhile backtracked out of that node. This guarantees the
two trails never intersect. In practice, the implementation maintains two trail heads `r`
and `b`, and the parent trail pointers are attached to the nodes as they are visited. The
two trail heads may collide, at which point backtracking is performed on `b` first by
convention.

Along the ddfs we may need to backtrack along one of the two trails. Being optimistic,
while advancing we can just take predecessor edges at will and hope we don't collide the
two trails, and then "backtrack" one of the two trails in case of collision, by convention
`b`. If the first backtracking fails, we try the second trail, and if that fails we found
a "barrier" node that is a common ancestor of all the nodes visited by the ddfs (and the
ones skipped over). Indeed all prop edges emanating from this barrier node are visited, to
ensure there is no way around it. Backtracking succeeds if the trail can get "around" the
collision point, and fails otherwise. Getting "around" means finding another reachable
node at the same or lower depth than the collision point. In case of failure the trail
head is commited to the collision point and its other trail's turn to backtrack.

To backtrack efficiently we maintain an arc index attached to each of the nodes, which
identify the next predecessor to take at that node after backtracking to it along a trail.
If the index is invalid, there are no more predecessors and further backtracking is
needed.

#### Saved data

The augmenting path subroutine will need to reuse the colored trails found during ddfs,
going backwards from the found exposed nodes. The blooms formed also maintain predecessor
(petal) and successor (trail) node relationships. Therefore to traverse the bloom we just
need to follow the pointers, and no dfs is required.

The bloom formation subroutine will need to perform dfs again (this time with no major
fuss) to assign maxlevels, bloom ids and identify new bridges. It will also need to form
two primary colored petals starting from the base.

Nothing is done by ddfs if red and blue were already in blooms with the same base.

Observation 1 (correctness optimization)

-   A node Q is unerased iff it has not been used in an augmenting path and there is still
    a path from it to an exposed vertex.
-   After augmentations, function `erase_successors()` erases nodes that have become
    unreachable from exposed vertices. Our ddfs implementation requires this procedure to
    be implemented properly - when advancing into a node u, there must be some path from
    that node to an exposed node.
-   The predecessors of a node we're going to advance to can be stored in arbitrary order.
    As such, as we iterate over the predecessor array of a node u, we can eliminate nodes
    that have been erased by swapping them with the last element.

#### Data structures

-   Three pointers: `r` (red, left), `b` (blue, right), `barrier` (for b).
-   One arc index per node
-   Four trail pointers per node, two for each color.
-   Given a node u, if the trail head is at u and a predecessor edge to v is taken, and
    w=base\*(v) is the next trail head, then we set

```
    w.trail.hi := u,   w.trail.lo := v.   w.arc := index of next(v)
```

Colored trail pointers are necessary for the bloom bases.

#### Algorithm

-   Advance whichever of r and b is further from the roots (i.e. largest min_level)
    advancing r in case of a tie.
    -   When advancing into nodes that belong to blooms skip directly to the bloom's base
        star (recursive base).
    -   Be careful not to advance into nodes that have been erased (by a previous
        augmentation in the same search) (observation 2)
    -   Notice that advancing casually (i.e. not while backtracking) only one step at a
        time guarantees the two paths can only cross at their endpoints (when `r=b`) and
        not somewhere in between. This is not necessarily true while backtracking either r
        or b as we usually need to backtrack and readvance several steps.
-   If `r=b` meet at the same node, this node is a bottleneck.
    -   Backtrack b first. Repeat, until there are no more nodes to search or the trail
        has gone around the bottleneck:
        -   Pop nodes from the trail until one has more predecessor nodes to go to, or
            until we reach the barrier.
        -   Advance the trail greedily along predecessor nodes, until the head collides
            with the other trail, revisits an old node, or goes around the trail.
        -   Backtracking fails when the head is equal to the barrier and there are no more
            predecessor edges; otherwise it succeeds. In this case, set barrier to this
            bottleneck and backtrack r.
-   If at any point both `r!=b` have no predecessors, i.e. they are exposed roots, we are
    done and found an augmenting path.
-   If neither trail can get around the bottleneck, the bottleneck is the base of a new
    bloom.

### Bloom formation and coloring

              red  (blue)          walk examples, without the expanded blooms:
            ____R  (B)___
       ____/   / \  |\   \___             - walk_peak (using bloom succ):
      /       /   \ | \      \            p->i->e->B       (blue)
     /       /     \|  \      \           q->x->c->B       (blue)
    a       b      (c) (d)    (e)         r->n->h->b->R     red
    |\_   _/|\       \ / \___  |
    |  \_/  | \___   (x)     \ |          - walk_base (using bloom pred):
    |__/ \__|     \  / \      \|          R->b->h->n->t     red
    f       g      h/   \     (i)         B->c->x->q       (blue)
    |      /|    _/|\    \     |
    |     / |  _/ /| \    \    |          - walk_down (using whatever):
    |    /  |_/  / |  \    \   |          g->l->q
    j   k   l   /  m   n   (o)(p)         a->f->j->q
     \   \  |  /   |  /|  _/  /           d->i->p->t
      \   \ | /   _|_/ | /   /
       \___\|/   / |   |/___/         bloom succ of b: R
           (q)  r  s   t              bloom succ of c: B
             \  |  |  /               bloom succ of i: d or e, doesn't matter
              \ |  | /                bloom pred of h: n
               [base]                 bloom pred of c: x

#### After finding a bloom

If a bloom is found we identify:

-   the peak bridge (e)
-   the base (barrier)

All nodes reachable from the peak through predecessor edges up to the barrier have been
visited and properly colored. All these nodes constitute the bloom. Every node u in this
bloom has been colored red or blue such that: a) If u is red there is a red path from u to
the red peak node. If u is blue there is a blue path from u to the blue peak node. b)
There is a red path from the red peak to the base (primary red petal). There is a blue
path from the blue peak to the base (primary blue petal). c) If u is red there isn't
necessarily a red path from u to the base. If u is blue there isn't necessarily a blue
path from u to the base.

a) This invariant allow us to find a path from any node u to a peak using only one color.
If an augmenting path is found through the bloom that needs to go "around" the bloom, this
path is used to go from the first node to the peak, making sure to use only one color as
another disjoint path going down must be found too. We optimistically assume such an
augmenting path will be found, so to facilitate its construction, we assign bloom
successors to every node alongside their maxlevels. If `P(u)` is a path of u's color from
u to the peak, then the bloom successor of u is the "guide" prop edge e that leads to the
second node in `P(u)`. Suppose ddfs, to reach u, went from vertex v through edge e,
pointing to w. Then `u=base*(w)` and the successor prop of u is edge e. The second node in
`P(u)` is v, before expanding blooms. Storing the edge e instead of v allows us to know
the vertex w immediately to expand the blooms up to u.

b) These paths correspond to the red and blue trails constructed during ddfs. We didn't
store these, but we can recover them quickly with a naive dfs following predecessors of
the same color. If an augmenting path is found through the bloom that needs to go "around"
the bloom, this path is used to go down from the peak to the base, making sure to use the
opposite color of that used to go from the first node to the peak. To identify this
"primary petal" we assign bloom predecessors to the nodes on it. Only one such petal is
constructed for each color.

c) This happens because the ddfs might backtrack successfully more than one node, leaving
for example a blue node with only red predecessor nodes. Such nodes were removed from the
trail but are still part of the bloom. If an augmenting path is found through the bloom
that needs to "descend" the bloom directly, i.e. not go "around" it, then the colors don't
matter as only a single subpath through the bloom is needed, not two disjoint ones.

Be careful not to touch the bloom's base in any way.

#### Algorithm

-   To form the primary red and blue petals, walk the trail pointers starting at the base.
    Both color trails have been filled.
-   Perform dfs to assign maxlevels, bloom ids, and identify bridges.
    -   Visit only pred nodes of the same color.
    -   Avoid revisiting nodes reachable in multiple ways.
    -   For bloom nodes with even maxlevel, look for bridges out of the bloom.

### Path augmentation and bloom expansion

    a
     \
      \  5~ ~ ~6
       \ |     |    b            going down the bloom:
        \|     |   /               a-->3-->1-->base
         3     4  /                   \_____/
         ~     ~ /                   walk_down()
         ~     ~/
         1     2                 going around the bloom:
          \   /                    b-->2-->4-->6-->5-->3-->1-->base
           \ /                        \_________/ \_________/
           base                        walk_peak() walk_base()

#### After finding an augmenting path

If an augmenting path is found we identify: - the peak bridge `e` - the red and blue
trails, properly colored, including exposed nodes the trails can be walked starting at the
exposed nodes

Each trail red/blue identifies a sequence of nodes `t=u(0),u(1),u(2),...,u(n)` such that t
is the red/blue peak and u(n) is an exposed vertex. The two sequences are disjoint by
construction.

#### Algorithm

-   For each trail red/blue with color c and top t:
-   Expand the initial bloom-star of t, if it exists.
-   Walk the trail, starting at u(n). Expand the bloom-star contracted in
    `[trail[u(i)][c].lo, u(i)]`, then append u(i), for `i=n,n-1,...,1`, and finally append
    t.
-   Be careful to maintain the path's correct direction throughout.
-   Join the two expanded paths.
-   Augment the constructed path by inverting all edges along it.
-   Erase all nodes in the path and all of their successors who can no longer be reached
    by any exposed node, recursively.

#### Node erasure

-   Using lazy erasure, erase only the first element of the successor's pred list until it
    is an unerased node. If the pred list becomes empty as a result the successor is to be
    erased as well.
-   Use lazy erasure also in ddfs to avoid entering erased nodes.

---

## Simplex

In `linear/simplex.hpp` we solve linear programming problems over the rationals. All
problems are maximization problems, for minimization negate the objective function.

The `simplex<F>` class owns the problem (like flow).

We rely on `numeric/frac.hpp` or `numeric/bfrac.hpp` for fractions and `linear/matrix.hpp`
for the tableau and pivot operations.

Using `frac` may lead to overflow issues for big linear programs as the denominators grow.
Using `bfrac` has no such issues but with considerable performance penalty. This is a
standard tradeoff; pick whichever is appropriate.

All linear problems are properly handled, including unbounded and impossible problems,
basis degeneracy, and all constraint types.

```
    Maximize cx
    Subject to
        Ax {<= == >=} b
        x >= 0

    N variables, M constraints
    all numbers are rational
    all coefficients (a, b, c) can be any rational (negative, zero, positive)
```

Constraints can be of types `<=` (LESS), `==` (EQUAL) and `>=` (GREATER).

Currently `x >= 0` is required for all variables. Support for `x <= 0` and `x ∈ R` is
pending still.

Tableau layout:

```
          (N) standard variables    (S) slack vars   (A) artif vars
    +----+-------------------------+----------------+-------------+
    |  0 | -z1  -z2  -z3  -z4  -z5 |  0  0  0  0  0 |  0  0  0  0 |
    | b1 | a11  a12  a13  a14  a15 |  1  0  0  0  0 |  1  0  0  0 |
    | b2 | a21  a22  a23  a24  a25 |  0  1  0  0  0 |  0 -1  0  0 |
    | b3 | a31  a32  a33  a34  a35 |  0  0 -1  0  0 |  0  0  0  0 |
    | b4 | a41  a42  a43  a44  a45 |  0  0  0  1  0 |  0  0  1  0 |
    | b5 | a51  a52  a53  a54  a55 |  0  0  0  0  1 |  0  0  0  0 |
    | b6 | a61  a62  a63  a64  a65 |  0  0  0  0  0 |  0  0  0 -1 |
    +----+-------------------------+----------------+-------------+

    tab[0][0]           Objective function value
    tab[0][1..]         Objective function coefficients (negated)
    tab[1..][0]         Constraint bound / Basis variable value
    tab[1..][1..]       Constraint coefficients

    b is positive    | slack | artif |
        <= -b            1      -1
        <=  0            1       0
        <=  b            1       0
        == -b            0      -1
        ==  0            0       1  --> simplification
        ==  b            0       1
        >= -b           -1       0
        >=  0           -1       0
        >=  b           -1       1

    artificial variables are set to b for initial feasible solution
```

The simplex solver is based on the two-phase method: - Phase 1 finds a feasible solution
that zeroes out all artificial variables - Phase 2 optimizes the feasible solution

First the solver counts the number of required slack and artificial variables according to
the rules above. Then the tableau is built as above, with slack and artificial variables
added as it goes.

If a row has an artificial variable then it is made the basic variable of the row;
Otherwise, the slack variable is made basic if it exists; Otherwise, the row would have no
associated basic variable initially; it appears this does not work with the current
implementation of `optimize()`, so the simplification above is used instead (a fake
artificial variable is added).

If there are no artificial variables then phase 1 can be skipped. If phase 1 is not
skipped then the original objective row is not added to the tableau until phase 2, where
it replaces the artificial objective row.

In phase 1 the objective is to minimize the sum of artificial variables; because they
start of as non-zero the artificial objective row must be _expanded_ first.

Afterwards the objective row is cleared and the original objective is expanded.

During tableau optimization we use Bland's rule to avoid cycling. We select the pivot
column `c` as the one where the objective is minimum (and negative) and the pivot row `r`
as the one where `tab[i][0] / tab[i][c]` is minimal, `tab[i][c] > 0` and break ties by
selecting the row where the basic variable is minimal.

At the end of phase 1 the tableau pivots on every basic artificial variable to remove them
all from the basis in case of degeneracy. The pivot columns do not matter as the variable
values will not change, so the first is available is chosen.

Notice that in this implementation the number of tableau columns (variables) in
`optimize()` is not necessarily larger than the number of rows (constraints), and some
rows may not be basic in that case. Bland's rule prefers these rows if possible; I'm not
sure if this is correct.

---

## Link Cut Tree

We implement link cut trees in `struct/link_cut_tree.hpp`. The implementation tested is
the classical add-update and sum-query where nodes maintain an integer value.

We support both oriented subtree queries and ancestor path queries. General path queries
can also be implemented "by hand" using the least common ancestor of the two nodes. Idem
for updates.

We follow the left-hand rule in the implementation: going right in an internal splay tree
is going down in the represented tree (by default).

Nodes are numbered 1 to N. Node 0 is the head sentinel and should be filled with neutral
values (often not the default value for other nodes).

### Lazy propagation and rerooting - Implementation notes

To support tree rerooting we maintain a lazy flip boolean on each node. Notice that when
we reroot a tree previously rooted at $r$ on a node $u$, we flip exactly those edges on
the path from $u$ to $r$, i.e. the operation needs to be propagated on the splay tree that
is formed by connecting exactly these two nodes.

To support this `reroot` operation and coincidentally also the other queries, we need to
make a setup where the virtual tree's root splay tree is made up of exactly those nodes
from $r$ to $u$, and $u$ is the splay root. Notice that in this case, $u$ has no right
child.

We start by accessing $u$; this merges $u$'s splay tree with all its parent nodes, and
makes $u$ the virtual root. Then we *trim* $u$'s right child, i.e. we explicitly turn the
edge going down from $u$ into a light edge (if it wasn't already) and turn the right
subtree into a virtual subtree. Then, to reroot at $u$ we simply move its left child to
the right side, and to query $u$'s path aggregate we simply maintain the aggregate of just
$u$'s splay tree, etc.

### Aggregate operations

We can group the interface into three parts:
- The **core** interface which includes `link`, `cut`, `reroot`, `findroot` and `lca`.
- The **update** interface which includes `update_node`, `update_path` and
  `update_subtree`.
- The **query** interface which includes `query_subtree` and `query_path`, along with
  their size/length counterparts if desired.

While `update_node` can be implemented in a straightforward way by just calling `access`
and `apply`, the other two require lazy propagation akin to that on a segment tree.

Not many *update operations* can be supported and propagated lazily through the update
interface to change the value at each node. Some good examples include `+=` and `^=`, and
also `=` if we keep tracking subtree sizes. Other operations like `gcd` are not supported
for updates - use a different data structure in this case.

The *query operations* should of course be associative, and preferably commutative. In
particular, for subtree queries they need to be invertible, otherwise implementing
`[link|cut]_virtual_subtree` is impossible. Use top trees instead in this case.

- Updates are handled in the functions at the top:
    - `apply(u, value)`
        - Set a new value at node $u$, and update aggregates too.
    - `pushup(u)`
        - Update $u$'s aggregates from its splay children's and virtual aggregates.
    - `pushdown(u)`
        - Flush $u$'s lazy deltas to its splay children.
    - `link_virtual_subtree(u, c)`
        - Add $c$ as a virtual subtree of $u$. Should update $u$'s virtual aggregates.
    - `cut_virtual_subtree(u, c)`
        - Remove $c$ as a virtual subtree of $u$. Should update $u$'s virtual aggregates.
```
    What to include in the queries/aggregates?
    u.self = u's value | u.sub = subtree aggregate | u.vir = virtual aggregate
    Let u be a node, l the left child, r the right child, and virt a virtual child.

                          u.self   l.sub   r.sub   u.vir   virt.sub...
    aggregate subtree      yes      yes     yes     yes        no
    aggregate virtual      no       no      no      no         yes
    query subtree          yes      no      yes     yes        no
    query path             yes       ?       ?      no         no
```
- For **subtree queries**:
    - The operation must be **invertible**.
        - Examples: `+`, `xor`, `or`, ...
        - Not supported: `min`, `max`, `gcd`, ...
    - Virtual subtree aggregates must be tracked in `[link|cut]_virtual_subtree()`.
- For **path queries**:
    - The operation need not be invertible.

### Interface

We support all of the following. Below *sum* should be understood as *aggregate*.

- Link $O(\log N)$: `link(u, v)`
    - Standard link-cut operation. $u$ is the root of a tree and $v$ is a node.
    - Set the parent of $u$ to $v$.
- Cut $O(\log N)$: `cut(u, v)`
    - Standard link-cut operation: $u$ and $v$ are nodes and $v$ is the parent of $u$.
    - Remove the link from $u$ to $v$ and let $u$ become a new tree root.
- Point updates $O(1)$: `update_node(u, new_value)`
    - Change the value of $u$ to `new_value`.
- Subtree queries $O(\log N)$: `query_subtree(u)`
    - Compute aggregate of `value(w)` for every node $w$ in $u$'s subtree, inclusive.
- Subtree updates $O(\log N)$: `update_subtree(u, increment)`
    - Update lazily `value(w)` for every node $w$ in $u$'s subtree, inclusive.
- Subtree size $O(\log N)$: `subtree_size(u)`
    - Compute the size of $u$'s subtree, inclusive.
- Path queries $O(\log N)$: `query_path(u, v)`
    - Compute aggregate of `value(w)` from $u$ to $v$, where $v$ is an ancestor of $u$.
- Path updates $O(\log N)$: `update_path(u, v, increment)`
    - Update lazily `value(w)` from $u$ to $v$, where $v$ is ancestor of $u$.
- Path length $O(\log N)$: `path_length(u, v)`
    - Compute the length of the path from $u$ to $v$, where $v$ is an ancestor of $u$.
- Find root $O(\log N)$: `find_root(u)`
    - Find the root of $u$'s tree.
- Reroot $O(\log N)$: `reroot(u)`
    - Make $u$ the new root of its tree, *flipping* the orientation of all parent edges.
- Lowest common ancestor $O(\log N)$: `lca(u, v)`
    - Compute the lowest common ancestor of two nodes $u$ and $v$ (possibly inexistent).
