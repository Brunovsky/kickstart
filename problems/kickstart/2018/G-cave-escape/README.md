# Kickstart 2018 - G Cave Escape

## [Cave Escape (19pts, 27pts)](https://codingcompetitions.withgoogle.com/kickstart/round/0000000000051066/0000000000051135)

Solved

1. Preparation in `O(RC)` time:

Analyze the cave to find all the rooms and traps, and all adjacencies between
rooms/traps and traps/traps.

2. Construct a graph G with traps as nodes.

For convenience, add a trap with strength 0 at S, the starting position (I do not actually do this).
Connect two nodes if they are in adjacent cells in the cave or there is a room between them.

3. Unconstrained solution in O(2^T) time:

Let's first waive the requirement that our agent's energy can never be <0.
For every (possibly empty) set of traps `tmask` compute the amount of energy
`unconstrained[tmask]` that our agent has after walking through all of the traps
plus all the reachable rooms.
Connectivity between these rooms, traps and S/T, does not matter whatsoever.

4. Constrained connected path solution in O(T 2^T) time:

Suppose our agent has reached a position where he has visited the set of traps
`tmask` and has not yet left the cave. Then he has `E=unconstrained[tmask]` energy
because he'll pick up every potion along the way.

If he can exit the cave now then `constrained[tmask]>=E` as `E` would be a viable solution.
Otherwise the agent will still have to visit at least an extra trap, and may do
so anyway if his energy would increase.

Consider all the traps that are reachable still. Let `trap` be one such trap.
If `E` is large enough to go through `trap`, then our agent may choose to do so
next and end up with `constrained[tmask U trap]` energy, which is good if it's
larger than `E`.
We do this for all reachable traps and we know the maximum energy the agent may get.
If no reachable trap can be visited because `E` is too low then he cannot exit
the cave through this path, so we signal `constrained[tmask]=-1`.

So, sets of traps with `n` traps depend on sets of traps with `n + 1` traps,
so a dynamic programming solution iterating over `n=T,T-1,...,1,0` will find
`constrained[tmask]` for every set. There are `2^T` sets of traps and each set
has to visit `O(T)` neighbour traps, hence the complexity.

Data set from [here](https://github.com/amylmy/KickStart-GoogleCompetition/blob/master/2018/RoundG/C_CaveEscape.java).

* Time: 16 hours
* Complexity: O(T 2^T + RC), where T is the number of traps
* Memory: O(2^T + RC)
