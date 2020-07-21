# Kickstart 2019 - A Contention

## [Contention (18pts, 27pts)](https://codingcompetitions.withgoogle.com/kickstart/round/0000000000050e01/0000000000069881)

Solved

Lazily-updated segment tree solution.

1. Data structure

Construct a segment tree the obvious way on the endpoints of the intervals. Then add
them to the segment tree. Each insert takes amortized time O(log(Q)).

On each node store four data points:

- contain: the number of intervals containing this entire subtree's range
- overlap: the number of intervals at least overlapping this entire subtree's range
- leaf: `minimum(node.overlap, minimum({positive overlaps in child nodes}))`
- books: the intervas (bookings) that contain this node but not the parent.

Notice that `node.contain >= node.books.size() >= node.overlap`.

When we insert an interval we increment `overlap` and perhaps `contain` and `leaf`.
Removing the intervals decrements these. However, these deltas affect the entire node's
subtree, so to avoid O(Q) propagation time of these values to the subtree we update
them lazily on-demand using a variable `delta` in O(1) time.

The total number of int ids in the book sets stored on each node will in total be about
O(Q log(Q)), so inserting all of them across all queries takes time O(Q log^2(Q)).
Idem for removing them.
The walkup algorithm takes time O(log(Q)) as we might go as far as the root, and the
search_leaf algorithm takes worst-case time O(Q). However, walkup on a node occurs at
most once, and search_leaf at most twice, so in total the queries still take time
O(Q log(Q)) and ironically the complexity is dominated by the operation of inserting
and removing integers from a set.....

1. Basic insight

Let `I` be the union of all booking intervals. Let `Bi` be a booking (i.e. an interval).
Suppose the last booking you enter into the system is `Bi`. Then `I\Bi` will be the
number of seats that `Bi` gets, which does not depend on the order of earlier bookings.

* Time: 15 hours
* Complexity: O(Q log^2(Q))
* Memory: O(Q log(Q))
