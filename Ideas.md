# Ideas

- Constructive
  - Look for overlay patterns, independent of particular input

- Look for randomized solutions
  - Need to find something such that *at least $\frac{N}{constant}$ elements*...

- Look for binary search simplifications
  - Can you binary search one dimension of the dp?

- Combinatorics
  - Look for *bijections* (of the entire problem or of subproblems)
  - Does the order matter? Maybe solve unordered then apply ordering.
  - Try to break cases apart (instead of summing nums >=1, sum portions=1 of nums)
  - If formula is too complicated then count a different way (you'll save time)
  - Different perspectives:
    - Event frequency (global or local)
    - Event count (global or local)
    - Event prefix count (global or local)
    - Probability
    - Inclusion-exclusion

- Dynamic programming
  - $n! \to 2^n$ look for SOS and TSP like approaches
  - SOS-like: ~ combinatorics
    - Compute downwards or upwards?
  - TSP-like: ~ graphs
    - The optimal value for `mask` must *encode* the position of its elements
    - Walk left to right, add new value to `mask` on the right side
  - Walk-like: ~ arrays
    - Instead of counting good cases, count all - bad cases (global or local)
