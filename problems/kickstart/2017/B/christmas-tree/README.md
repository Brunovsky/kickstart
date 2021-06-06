# Kickstart 2017 - Christmas Tree

## [Christmas Tree (11pts, 32pts)](https://codingcompetitions.withgoogle.com/kickstart/round/0000000000201d27/0000000000201c9a)

Solved

Obviously it's dynamic programming. Notice that a complexity of RCCK is
acceptable given the 100 bounds.

For every green cell G and every k <= K, store the largest K-christmas tree
whose lowest good triangle is seated in G (and includes G).

Also, for every green cell G store the height of the largest good triangle whose
root (top cell) is G.

Iterate the tree downwards. For every row, split it into column ranges
like `[cmin,cmax]` of all green cells surrounded by dots or by borders.
Iterate these ranges.
Visualize all the triangles that are seated in one such range.
The good triangles yield new solutions and must be visited. If a good triangle
is seated in `[c1,c2]` then its solution should update `dp[r][ci][k]` for all
ci in `[c1,c2]` and all k. Naively this would take O(c^3) which is not
good enough, but with clever ordering and caching we can reduce it to O(c^2),
which is good enough.

* Time: 5 hours
* Complexity: O(NKM^2)
* Memory: O(NMK)
