# Implementation notes

## Simplex

In `simplex.hpp` we solve linear programming problems over the rationals.
All problems are maximization problems, for minimization negate the objective function.

The `simplex` class owns the problem (like flow).

We rely on `frac.hpp` for fractions and `mat.hpp` for the tableau and pivot operations.

All linear problems are properly handled, including unbounded and impossible problems,
basis degeneracy, all constraint types

```
    Maximize cx
    Subject to
        Ax OP b
        x >= 0

    n variables, m constraints
    all numbers are rational
    all coefficients (a, b, c) can be any rational (negative, zero, positive)
```

Constraints can be of types `<=` (LESS), `==` (EQUAL) and `>=` (GREATER).

Currently `x >= 0` is required for all variables.
Support for `x <= 0` and `x ∈ R` is pending still.

Tableau layout:

```
          (n) standard variables    (s) slack vars   (a) artif vars
    +----+-------------------------+----------------+-------------+
    |  0 | -z0  -z1  -z2  -z3  -z4 |  0  0  0  0  0 |  0  0  0  0 |
    | b0 | a00  a01  a02  a03  a04 |  1  0  0  0  0 |  1  0  0  0 |
    | b1 | a10  a11  a12  a13  a14 |  0  1  0  0  0 |  0 -1  0  0 |
    | b2 | a20  a21  a22  a23  a24 |  0  0 -1  0  0 |  0  0  0  0 |
    | b3 | a30  a31  a32  a33  a34 |  0  0  0  1  0 |  0  0  1  0 |
    | b4 | a40  a41  a42  a43  a44 |  0  0  0  0  1 |  0  0  0  0 |
    | b5 | a50  a51  a52  a53  a54 |  0  0  0  0  0 |  0  0  0 -1 |
    +----+-------------------------+----------------+-------------+

    tab[0][0]           Objective function value
    tab[0][1..]         Objective function coefficients
    tab[1..][0]         Constraint bound / Basis variable value
    tab[1..][1..]       Constraint coefficients

    b is positive    | slack | artif |
        <= -b            1      -1
        <=  0            1       0
        <=  b            1       0
        == -b            0      -1
        ==  0            0       0
        ==  b            0       1
        >= -b           -1       0
        >=  0           -1       0
        >=  b           -1       1
```

The simplex solver is based on the two-phase method:
    - Phase 1 finds a feasible solution that zeroes out all artificial variables
    - Phase 2 optimizes the feasible solution

First the solver counts the number of required slack and artificial variables
according to the rules above. Then the tableau is built as above, with slack and
artificial variables added as it goes.

If a row has an artificial variable then it is made the basic variable of the row;
Otherwise, the slack variable is made basic if it exists;
Otherwise, the row has no associated basic variable initially.

If there are no artificial variables then phase 1 can be skipped.
If phase 1 is not skipped then the original objective row is not added to the
tableau until phase 2, where it replaces the artificial objective row.

In phase 1 the objective is to minimize the sum of artificial variables; because
they start of as non-zero the artificial objective row must be *expanded* first.

Afterwards the objective row is cleared and the original objective is expanded.

During tableau optimization we use Bland's rule to avoid cycling. We select the
pivot column `c` as the one where the objective is minimum (and negative) and the pivot
row `r` as the one where `tab[i][0] / tab[i][c]` is minimal and `tab[i][c] > 0`.

At the end of phase 1 the tableau pivots on every basic artificial variable to remove
them all from the basis in case of degeneracy. The pivot columns do not matter as the
variable values will not change.
