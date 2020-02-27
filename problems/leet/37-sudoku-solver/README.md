# LeetCode - hard

## [37. Sudoku Solver](https://leetcode.com/problems/sudoku-solver)

Solved

Backtracking (stacktracking) solution. Solution is designed specifically for
Sudoku, it's not a generic exact cover solver or Algorithm X implementation.

The algorithm stores *metadata* of the puzzle's state as a human would:
  * which numbers can still appear in any...
    * cell - `free`
    * square - `sqr`
    * row - `row`
    * column - `col`
  * how many cells have been filled - `count`

All cells which have only one possible attribution left are *cascaded* and the
attribution 'propagates' to neighbour cells at a later iteration of `assign`'s
loop, applying to them the restriction. Once no cells are left which have only
one possible attribution but the puzzle is not yet solved, a guess is made on
the first cell which has the least number of possible attributions (which is
usually two or three).

As usual, very hard puzzles can lead to considerable backtracking.
In most cases the solver which finish instantly, though.

* Complexity: O(∞) backtracking
* Memory: generous
