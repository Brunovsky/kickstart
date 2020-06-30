# Kickstart 2016 - Soldiers

## [Soldiers (16pts, 23pts)](https://codingcompetitions.withgoogle.com/kickstart/round/0000000000201c0a/0000000000201c0b)

Solved

Consider a graph G on the bounds (As, Ds) that are achievable through gameplay.
These bounds are basically the game's states, or positions.
Draw an edge from (A1, D1) to (A2, D2) if the player that receives bound
(A1, D1) can pick a soldier and give (A2, D2) to his opponent.
This graph has O(N^2) nodes and O(N^3) edges.
The end bound/position is (max(A), max(D)) which is a losing position.
A bound/position is losing if there is an edge from it to a winning position.
A bound/position is winning if all edges from it are to losing positions.

The trick is to not actually visit all the edges, and as usual in game theory
we traverse the position graph in reverse, from the end to the start.

* Time: 6 hours
* Complexity: O(N^2)
* Memory: O(N^2)
