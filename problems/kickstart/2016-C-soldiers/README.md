# Kickstart 2016 - Soldiers

## [Soldiers (16pts, 23pts)](https://codingcompetitions.withgoogle.com/kickstart/round/0000000000201c0a/0000000000201c0b)

Solved

Given a set S of picked soldiers, the state of the game is entirely determined
by (As,Ds), where As and Ds are the maximums in S.
There are O(N^2) such valid states (exactly N^2 if all A are different and all
D are different).

Consider a graph G on all these bounds/positions/states (As,Ds).
Draw an edge (A1,D1) --> (A2,D2) if the player that receives (A1,D1) can pick a
soldier and give (A2,D2) to his opponent.
This graph has O(N^2) nodes and O(N^3) edges.
The game ends when we reach (max(A),max(D)), so it is a losing position.

Perfect play:
A bound/position is losing if all edges from it are to a winning positions.
A bound/position is winning if any edge from it is to a losing positions.

The trick is to not actually visit nor represent all the edges, and as usual in
game theory we traverse the position graph in reverse topological order, from
the end position to the start position (0,0).

Mapping the set of attacks and defenses to 1..N is necessary to avoid using
a map, which is not fast enough for test set 2.

* Time: 6 hours
* Complexity: O(N^2)
* Memory: O(N^2)
