# Status check

This code collection has become quite messy. ✓✓XX

| Module              | File                        | Code Status  | Testing Status | Notes |
|:-------------------:|-----------------------------|--------------|:--------------:|-------|
| Algorithms          |`dynamic_programming        `| Stable       | Stable (U,S)   | Multibin knapsack, minimum bin packing (separate knapsack) |
| Algorithms          |`string_search              `| Stable       | Stable (S)     | Suffix array, suffix tree, manacher's algorithm |
| Algorithms          |`sort                       `| Stable       | Stable (P)     | |
| Binary trees        |`avl_tree                   `| Stable       |                | |
| Binary trees        |`bs_map                     `| Stable       |                | |
| Binary trees        |`bs_set                     `| Stable       |                | |
| Binary trees        |`bs_tree                    `| Stable       |                | Missing some libcpp methods with hints |
| Binary trees        |`rb_tree                    `| Stable       |                | |
| Binary trees        |`splay_tree                 `| Stable       |                | |
| Data structures     |`bigint                     `| Stable       |                | Convoluted, needs cleanup: use debug functions, improve distribution |
| Data structures     |`disjoint_set               `| Stable       |                | |
| Data structures     |`fenwick                    `| Stable       |                | |
| Data structures     |`frac                       `| Stable       |                | |
| Data structures     |`integer_data_structures    `| Stable       |                | Only heaps are performance tested on dijkstra |
| Data structures     |`lca                        `| Stable       |                | Needs restyle |
| Data structures     |`matrix                     `| Stable       |                | |
| Data structures     |`rmq                        `| Stable       |                | |
| Data structures     |`segment_tree               `| Template     |                | It's a template but could be basic tested |
| Data structures     |`simple_data_structures     `| Stable       |                | |
| Flow                |`maximum_flow               `| Stable       |                | |
| Flow                |`mincost_circulation        `| Outdated     |                | Not adapted to new edges_t graph interface |
| Flow                |`mincost_flow               `| Outdated     |                | Not adapted to new edges_t graph interface |
| Flow                |`mincut                     `| Outdated     |                | Not adapted to new edges_t graph interface |
| Geometry            |`geometry2d                 `| Stable       |                | |
| Geometry            |`convex_hull                `| Stable       |                | |
| Graphs              |`graph_formats              `| Incomplete   |                | Standardized reading and writing, restyle |
| Graphs              |`graph_generator            `| Incomplete   |                | Missing flow, circulation, shortest paths, matching and planarity generators |
| Graphs              |`isomorphism                `| Outdated     |                | Not adapted to new edges_t graph interface |
| Graphs              |`regular                    `| Stable       |                | |
| Graphs algorithms   |`min_spanning_forest        `| Outdated     |                | Not adapted to new edges_t graph interface |
| Graphs algorithms   |`planarity                  `| Outdated     |                | Not adapted to new edges_t graph interface |
| Graphs algorithms   |`scc                        `| Outdated     |                | Not adapted to new edges_t graph interface |
| Graphs algorithms   |`shortest_path              `| Stable       |                | Johnson's not correctly implemented |
| Linear programming  |`simplex                    `| Stable       |                | Missing support for x <= 0 and x in R |
| Linear programming  |`transportation             `| Inadequate   |                | Inefficient implementation and bad interface |
| Linear programming  |`travelling_salesman        `| Outdated     |                | Needs an interface change |
| Matching            |`general_matching           `| Stable       |                | |
| Matching            |`maximum_matching           `| Stable       |                | |
| Matching            |`mincost_matching           `| Outdated     |                | Not adapted to new edges_t graph interface |
| Threading           |`thread_pool                `| Stable       |                | |
| Utilities           |`bits                       `| Stable       |                | |
| Utilities           |`debug_print                `| Stable       |                | |
| Utilities           |`hash                       `| Stable       |                | |
| Utilities           |`math                       `| Stable       |                | |
| Utilities           |`primes                     `| Stable       |                | |
| Utilities           |`random                     `| Stable       |                | |

Other files: `tree_core` `graph` `graph_operations`

Test types:

- Unit tests (problems defined inline; untimed; correct answer known)
  - `unit_test_X()`
- Dataset tests (problems read from a file; untimed; correct answer known)
  - `dataset_test_X()`, `dataset_test_X_run()`, `read_dataset_test()`
- Stress tests (problems generated; untimed; verified)
  - `stress_test_X()`, `stress_test_X_run()`
- Speed tests (problems generated; timed; unverified)
  - `speed_test_X()`, `speed_test_X_run()`
- Scaling tests (problems generated; timed; verified; complexity scaling checked)
  - `scaling_test_X()`, `scaling_test_X_run()`
- Visual tests (problems generated or defined inline)
  - `format_test_X()`

## Testing

| Code                     | Status | Notes |
|--------------------------|:------:|-------|
|`planarity               `|  XXXX  |       |
|`maximum_matching        `|  XXXX  |       |
|`mincost_matching        `|  XXXX  |       |
|`general_matching        `|  XXXX  |       |
|`mincost_general_matching`|  -     | Not implemented yet |
|`shortest_path           `|  XXXX  |       |
|`maximum_flow            `|  XXXX  |       |
|`mincost_flow            `|  XXXX  |       |
|`mincost_circulation     `|  XXXX  |       |
|`simplex                 `|  XXXX  |       |
|`transportation          `|  XXXX  |       |
|`mincut                  `|  XXXX  |       |
