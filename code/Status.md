# Status check

This code collection has become quite messy. ✓✓XX

| Module              | File                        | Code Status  | Testing Status | Notes |
|:-------------------:|-----------------------------|--------------|:--------------:|-------|
| Algorithms          |`dynamic_programming        `| Stable       | Stable (U,S)   | Add: Multibin knapsack, minimum bin packing (separate knapsack) |
| Algorithms          |`string_search              `| Stable       | Stable (S)     | Add: Suffix array, suffix tree, manacher's algorithm |
| Algorithms          |`sort                       `| Stable       | Stable (P)     | |
| Binary trees        |`avl_tree                   `| Stable       | -              | |
| Binary trees        |`bs_map                     `| Stable       | Stable (U,S,P) | |
| Binary trees        |`bs_set                     `| Stable       | Stable (U,S,P) | |
| Binary trees        |`bs_tree                    `| Stable       | -              | Missing some libcpp methods with hints |
| Binary trees        |`rb_tree                    `| Stable       | -              | |
| Binary trees        |`splay_tree                 `| Stable       | -              | |
| Data structures     |`bigint                     `| Stable       | Stable (U,S)   | |
| Data structures     |`disjoint_set               `| Stable       | -              | |
| Data structures     |`fenwick                    `| Stable       | Stable (U)     | |
| Data structures     |`frac                       `| Stable       | Stable (U)     | |
| Data structures     |`integer_lists              `| Stable       | Stable (U,S)   | |
| Data structures     |`integer_heaps              `| Stable       | Stable (U,P)   | |
| Data structures     |`lca                        `| Stable       | Stable (U)     | |
| Data structures     |`matrix                     `| Stable       | Stable (U,S,P) | |
| Data structures     |`rmq                        `| Stable       | Stable (U)     | |
| Data structures     |`segment_tree               `| Stable       | Stable (U)     | Add: Dynamic sparse segment tree |
| Data structures     |`simple_data_structures     `| Stable       | -              | |
| Flow                |`maximum_flow               `| Stable       | Stable (S,P)   | Class generator imbalanced |
| Flow                |`mincost_circulation        `| Outdated     |                | Not adapted to new edges_t graph interface |
| Flow                |`mincost_flow               `| Stable       | Stable (D,P)   | |
| Flow                |`mincut                     `| Stable       | Missing        | Plenty of room for optimization |
| Geometry            |`geometry2d                 `| Stable       | -              | |
| Geometry            |`convex_hull                `| Stable       | -              | |
| Graphs              |`graph_formats              `| Incomplete   |                | Standardized reading and writing, restyle |
| Graphs              |`graph_generator            `| Incomplete   |                | Add: general matching and planarity generators |
| Graphs              |`isomorphism                `| Stable       | Stable (S)     | |
| Graphs              |`regular                    `| Stable       |                | |
| Graphs algorithms   |`min_spanning_forest        `| Stable       | Stable (U)     | |
| Graphs algorithms   |`planarity                  `| Stable       | Stable (D)     | |
| Graphs algorithms   |`scc                        `| Stable       | Stable (U)     | |
| Graphs algorithms   |`shortest_path              `| Stable       | Stable (P)     | Johnson's not correctly implemented, Goldberg needs verification |
| Linear programming  |`simplex                    `| Stable       |                | Missing support for x <= 0 and x in R |
| Linear programming  |`transportation             `| Inadequate   |                | Inefficient implementation and bad interface |
| Linear programming  |`travelling_salesman        `| Stable       | Stable (D)     | |
| Matching            |`general_matching           `| Stable       | Stable (D,S,P) | |
| Matching            |`maximum_matching           `| Stable       |                | |
| Matching            |`mincost_matching           `| Stable       | Stable (U,P)   | |
| Threading           |`thread_pool                `| Stable       |                | |
| Utilities           |`bits                       `| Stable       |                | |
| Utilities           |`debug_print                `| Stable       |                | |
| Utilities           |`hash                       `| Stable       |                | |
| Utilities           |`math                       `| Stable       |                | |
| Utilities           |`primes                     `| Stable       |                | |
| Utilities           |`random                     `| Stable       |                | |

Test types:

- (U) Unit tests (problems defined inline; untimed; correct answer known)
 `unit_test_X()`
- (D) Dataset tests (problems read from a file; untimed; correct answer known)
 `dataset_test_X()`, `dataset_test_X_run()`, `read_dataset_test()`
- (S) Stress tests (problems generated; untimed; verified)
 `stress_test_X()`, `stress_test_X_run()`
- (P) Speed tests (problems generated; timed; maybe verified)
 `speed_test_X()`, `speed_test_X_run()`
- (A) Scaling tests (problems generated; timed; unverified; complexity scaling checked)
 `scaling_test_X()`, `scaling_test_X_run()`
- (V) Visual tests (problems generated or defined inline)
 `format_test_X()`
