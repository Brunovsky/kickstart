# Status check

This code collection has become quite messy

| Module              | Code                        | Status       | Testing | Notes |
|:-------------------:|-----------------------------|--------------|:-------:|-------|
| Algorithms          |`dynamic_programming        `| Incomplete   |   ✓     | Many algorithms and variations still missing |
| Algorithms          |`string_search              `| Stable       |   ✓✓    | Needs restyle; suffix array, suffix tree and manacher's algorithm |
| Algorithms          |`sort                       `| Stable       |   ✓✓✓   | Needs restyle, disorganized |
| Binary trees        |`avl_tree                   `| Stable       |   ✓✓✓   | |
| Binary trees        |`bs_map                     `| Stable       |   ✓✓✓   | |
| Binary trees        |`bs_set                     `| Stable       |   ✓✓✓   | |
| Binary trees        |`bs_tree                    `| Stable       |   ✓✓✓   | Missing some libcpp methods with hints |
| Binary trees        |`rb_tree                    `| Stable       |   ✓✓✓   | |
| Binary trees        |`splay_tree                 `| Stable       |   ✓✓✓   | |
| Data structures     |`bigint                     `| Stable       |   ✓✓    | Convoluted, needs cleanup: use debug functions, improve distribution |
| Data structures     |`disjoint_set               `| Stable       |   X     | |
| Data structures     |`fenwick                    `| Stable       |   ✓     | |
| Data structures     |`frac                       `| Stable       |   ✓     | |
| Data structures     |`integer_data_structures    `| Stable       |   ✓✓✓   | Only heaps are performance tested on dijkstra |
| Data structures     |`lca                        `| Stable       |   ✓     | Needs restyle |
| Data structures     |`matrix                     `| Stable       |   ✓     | |
| Data structures     |`rmq                        `| Stable       |   ✓     | |
| Data structures     |`segment_tree               `| Template     |   X     | It's a template but could be basic tested |
| Data structures     |`simple_data_structures     `| Stable       |   X     | |
| Flow                |`maximum_flow               `| Stable       |   ✓✓✓   | |
| Flow                |`mincost_circulation        `| Outdated     |   ✓     | Not adapted to new edges_t graph interface |
| Flow                |`mincost_flow               `| Outdated     |   ✓✓✓   | Not adapted to new edges_t graph interface |
| Flow                |`mincut                     `| Outdated     |   ✓     | Not adapted to new edges_t graph interface |
| Geometry            |`geometry2d                 `| Stable       |   ✓     | |
| Geometry            |`convex_hull                `| Stable       |   ✓     | |
| Graphs              |`graph_formats              `| Incomplete   |         | Standardized reading and writing, restyle |
| Graphs              |`graph_generator            `| Incomplete   |   ✓✓    | Missing flow, circulation, shortest paths, matching and planarity generators |
| Graphs              |`isomorphism                `| Outdated     |   ✓✓    | Not adapted to new edges_t graph interface |
| Graphs              |`regular                    `| Stable       |   ✓✓✓   | |
| Graphs algorithms   |`min_spanning_forest        `| Outdated     |   ✓     | Not adapted to new edges_t graph interface |
| Graphs algorithms   |`planarity                  `| Outdated     |   ✓     | Not adapted to new edges_t graph interface |
| Graphs algorithms   |`scc                        `| Outdated     |   ✓     | Not adapted to new edges_t graph interface |
| Graphs algorithms   |`shortest_path              `| Stable       |   ✓✓✓   | |
| Linear programming  |`simplex                    `| Outdated     |   ✓     | Needs an interface change |
| Linear programming  |`transportation             `| Inadequate   |   ✓     | Inefficient implementation and bad interface |
| Linear programming  |`travelling_salesman        `| Outdated     |   ✓     | Needs an interface change |
| Matching            |`general_matching           `| Stable       |   ✓✓✓   | Commented out unit tests |
| Matching            |`maximum_matching           `| Outdated     |   ✓     | Not adapted to new edges_t graph interface |
| Matching            |`mincost_matching           `| Outdated     |   ✓✓✓   | Not adapted to new edges_t graph interface |
| Threading           |`thread_pool                `| Stable       |   ✓✓✓   | |
| Utilities           |`bits                       `| Stable       |         | |
| Utilities           |`debug_print                `| Outdated     |         | Add helpful print functions for testing |
| Utilities           |`hash                       `| Stable       |   ✓     | |
| Utilities           |`math                       `| Stable       |   ✓     | Add dynamic modnum |
| Utilities           |`primes                     `| Stable       |   ✓✓    | No test feedback |
| Utilities           |`random                     `| Outdated     |   ✓✓    | Use {fmt} |

Testing:
- XX ⇒ not testable or not correct
- X ⇒ there are no tests
- ✓ ⇒ unit tests with hand-written input and output
- ✓✓ ⇒ stress tests with random input, checked for correctness algorithmically
- ✓✓✓ ⇒ timed performance tests with random input, maybe compared

Other files: `tree_core` `graph` `graph_operations`
