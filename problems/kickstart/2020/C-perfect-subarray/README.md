# Kickstart 2020 - Perfect Subarray

## [Perfect Subarray (10pts, 19pts)](https://codingcompetitions.withgoogle.com/kickstart/round/000000000019ff43/00000000003381cb)

Solved. Implicit (recursion) prefix/suffix sum tree.
Given the array A with size N, split it in half right down the middle,
into subarrays AL and AR.
Recurse on AL and on AR, counting the squares only on those subarrays.
The only squares not counted are then the ones who subarray crosses the split
boundary, i.e. including indices from both AL and AR.
But now we only need to care about the sums of the suffixes AL and the sums
of the prefixes of AR. There are N/2 of each on the first iteration (then
recursion).

This is basically the old problem of finding two indices i,j in a sorted array
whose contents' sum yield a predefined value v, except we have two arrays.
The predefined values are the ~3200 squares.

The number of squares is small enough for this method to beat test set 2.

* Time: 75 minutes
* Complexity: O(S N log N), where S is the number of squares
* Memory: O(S + N)
