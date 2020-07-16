# Kickstart 2017 - Trash

## [Trash (17pts, 27pts)](https://codingcompetitions.withgoogle.com/kickstart/round/0000000000201b77/0000000000201d23)

Solved

Initial solution used trigonometric functions to do ternary search on the slope
through O. New solution does ternary search on X directly, much better.

Find `R` with binary search. Pick and fix `R` for now.
Pick an obstacle and fix its coordinates `O=(Xo,Yo)`.
What is the value of `a=a(O)` such that `f(x)` passes exactly through `O`?
  `Yo = aXo(Xo-P) => a = Yo/(Xo(P-Xo))`
Fix a parabola not through `O` and imagine a circle G growing around `O`.
The circle will touch the parabola at some radius `r`. This means that a ball
thrown along this parabola can have radius <= `r`.
The point `O` and the point of contact will be one the same side of the
parabola's axis. Assume the side `x>=P/2`.
Given the radius `R` and the obstacle `O`, we can find the two parabolas above
and below `O` that are as close as possible to `O` and allow a ball of
radius `R` to be thrown.
We find this with binary search on `a` as well.
Given a parabola and an obstacle `O`, to find the distance `r` of `O` to the
parabola we can notice that as X varies from P/2 to P, the distance to `O`
traces a V-like function (imagine the circle G growing). This means we can find
`X` such that `OX` is minimized with ternary search, and the minimum is `r`.

* Time: 7 hours
* Complexity: O(N(30 + log P))
* Memory: O(N)
