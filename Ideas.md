# Ideas

- Look for randomized solutions
  - Need to find something such that *at least $\frac{N}{constant}$ elements*...

- Look for binary search simplifications
  - Can you binary search one dimension of the dp?

- Combinatorics
  - Does the order matter? Maybe solve unordered then apply ordering.
  - Try to break cases apart (instead of summing nums >=1, sum portions=1 of nums)
  - Different perspectives:
    - Frequency (global or local)
    - Count (global or local)
    - Up to count (global or local)

- Dynamic programming
  - $n! \to 2^n$ look for SOS and TSP like approaches
  - SOS-like: ~ combinatorics
    - Compute downwards or upwards?
  - TSP-like: ~ graphs
    - The optimal value for `mask` must *encode* the position of its elements
    - Walk left to right, add new value to `mask` on the right side
