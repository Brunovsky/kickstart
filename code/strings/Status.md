# Status table

| File                        | Code Status  | Testing Status | Notes |
|-----------------------------|--------------|:--------------:|-------|
|`aho_cosarick               `| Stable       | Stable (U)     | |
|`boyer_moore                `| Stable       | Stable (S)     | |
|`kmp                        `| Stable       | Stable (S)     | |
|`rabin_karp                 `| Stable       | Stable (S)     | |
|`strings                    `| Stable       | Stable (U,Z)   | |
|`string_distance            `| Stable       | Stable (U)     | |
|`suffix_array               `| Stable       | -              | |
|`suffix_automaton           `| Stable       | Stable (S)     | Inline transition table -- O(AN) mem, O(1) everything, O(N) traversal |
|`map_suffix_automaton       `| Stable       | Stable (S)     | Per-node map -- O(N) fat mem, O(log A) everything, O(N) traversal |
|`sparse_suffix_automaton    `| Stable       | Stable (S)     | Per-node linked list -- O(N) low mem, O(A) everything, O(N) traversal |
|`vector_suffix_automaton    `| Stable       | Stable (S)     | Per-node vector -- O(N) avg mem, O(log A) everything, O(N) traversal |
|`suffix_tree                `| TODO :(      |                | |
|`z_search                   `| Stable       | Stable (S)     | |
