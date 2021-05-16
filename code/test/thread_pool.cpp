#include "test_utils.hpp"
#include "../parallel/thread_pool.hpp"
#include "../lib/graph_generator.hpp"
#include "../linear/matrix.hpp"
#include "../numeric/bits.hpp"
#include "../parallel/fn_orchestrator.hpp"
#include "../parallel/graph_orchestrator.hpp"
#include "../parallel/priority_thread_pool.hpp"
#include "../parallel/spinlock.hpp"

inline namespace detail {

using mat_t = mat<long>;

mat_t makemat(int n, default_random_engine& rnd) {
    longd distv(-100, 100);
    mat_t mat(n, n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[i][j] = distv(rnd);
    return mat;
}

// Choose a random n and generate two nxn matrices and multiply them, discard the result
void compute(int id) {
    intd distn(200, 500);
    default_random_engine rnd(random_device{}());
    int n = distn(rnd);
    auto a = makemat(n, rnd);
    auto b = makemat(n, rnd);
    auto c = a * b;
    string s = format(" {:4} product complete, n: {:3}", id, n);
    puts(s.data());
}

void compute_priority(int id, int priority) {
    intd distn(200, 500);
    default_random_engine rnd(random_device{}());
    int n = distn(rnd);
    auto a = makemat(n, rnd);
    auto b = makemat(n, rnd);
    auto c = a * b;
    string s = format(" {:4} product complete, n: {:3}, p: {:4}", id, n, priority);
    puts(s.data());
}

template <typename T>
struct action_t {
    int b1, b2;
    T value;
};

} // namespace detail

inline namespace stress_testing_pool {

void stress_test_pool_submit(int N = 200, int rate = 47, int nthreads = 5) {
    thread_pool pool(nthreads);
    assert(pool.pool_size() == nthreads);
    int i = 1;
    print("stress test iterative submit\n");
    while (i <= N) {
        int j = 0;
        while (i <= N && j < rate) {
            pool.submit(compute, i), i++, j++;
        }
        string s = format("-- {:4}..{:4}, added {:4} jobs", i - j, i - 1, j);
        puts(s.data());
        if (j == rate) {
            pool.wait();
        } else {
            pool.cancel();
        }
    }
}

void stress_test_priority_pool_submit(int N = 200, int rate = 47, int nthreads = 5) {
    priority_thread_pool<int> pool(nthreads);
    assert(int(pool.pool_size()) == nthreads);
    int i = 1;
    intd priorityd(1000, 9999);
    print("stress test iterative submit with priority\n");
    while (i <= N) {
        int j = 0;
        while (i <= N && j < rate) {
            int p = priorityd(mt);
            pool.submit(p, compute_priority, i, p), i++, j++;
        }
        string s = format("-- {:4}..{:4}, added {:4} jobs", i - j, i - 1, j);
        puts(s.data());
        if (j == rate) {
            pool.wait();
        } else {
            pool.cancel();
        }
    }
}

} // namespace stress_testing_pool

inline namespace stress_testing_fn_orchestrator {

void fn_orchestrator_stress_run(int V, int buckets, int nthreads) {
    vector<vector<int>> vis(buckets), vis2(buckets);
    vector<action_t<int>> val(V);

    intd buckd(0, buckets - 1);
    intd numd(1, 100'000'000);
    for (int i = 0; i < V; i++) {
        val[i] = {buckd(mt), buckd(mt), numd(mt)};
    }

    auto job = [&](int u) {
        auto [b1, b2, num] = val[u];
        vis[b1].push_back(num);
        vis[b2].push_back(num);
    };

    auto depends = [&val](int u, int v) {
        int b1 = val[u].b1, b2 = val[u].b2, b3 = val[v].b1, b4 = val[v].b2;
        return u < v && (b1 == b3 || b1 == b4 || b2 == b3 || b2 == b4);
    };

    fn_orchestrator orch(V, depends);

    orch.sequential_make(job); // -> vis2
    swap(vis, vis2);
    orch.concurrent_make(job, nthreads); // -> vis

    int S = 0;
    for (int b = 0; b < buckets; b++) {
        S += vis[b].size();
    }

    if (vis != vis2) {
        fail("Different visitors");
    }
    if (S != 2 * V) {
        fail("Bad count: {}/{}", S, 2 * V);
    }
}

void fn_orchestrator_speed_run(int V, int n, int buckets, int nthreads) {
    vector<mat_t> vis(buckets, mat_t::identity(n)), vis2(buckets, mat_t::identity(n));
    vector<action_t<mat_t>> val(V);

    intd buckd(0, buckets - 1);
    intd numd(1, 100'000'000);
    for (int i = 0; i < V; i++) {
        val[i] = {buckd(mt), buckd(mt), makemat(n, mt)};
    }

    auto job = [&](int u) {
        auto [b1, b2, mat] = val[u];
        vis[b1] = vis[b1] * mat;
        vis[b2] = vis[b2] * mat;
    };

    auto depends = [&val](int u, int v) {
        int b1 = val[u].b1, b2 = val[u].b2, b3 = val[v].b1, b4 = val[v].b2;
        return u < v && (b1 == b3 || b1 == b4 || b2 == b3 || b2 == b4);
    };

    fn_orchestrator orch(V, depends);

    print("\r [{}] x{}  ({} buckets, {} threads)\n", n, V, buckets, nthreads);

    START(sequential);
    orch.sequential_make(job);
    TIME(sequential);
    PRINT_TIME(sequential);

    swap(vis, vis2);

    START(concurrent);
    orch.concurrent_make(job, nthreads);
    TIME(concurrent);
    PRINT_TIME(concurrent);

    if (vis != vis2) {
        fail("Different matrices");
    }
}

void stress_test_fn_orchestrator(int T = 100) {
    for (int t = 1; t <= T; t++) {
        print_progress(t, T, "stress test fn orchestrator");
        fn_orchestrator_stress_run(600, 40, 32);
    }
}

void speed_test_fn_orchestrator(int T = 25) {
    for (int t = 0; t < T; t++) {
        print_progress(t, T, "speed test fn orchestrator");
        fn_orchestrator_speed_run(1000, 70, 3 * (t + 1), 16);
    }
}

} // namespace stress_testing_fn_orchestrator

inline namespace stress_testing_graph_orchestrator {

void graph_orchestrator_stress_run(int V, int E, int nthreads) {
    vector<unsigned long> vis(V, 1), vis2(V, 1);

    auto g = random_exact_rooted_dag_connected(V, E);
    auto rev = make_adjacency_lists_reverse(V, g);

    auto job = [&](int u) {
        for (int v : rev[u])
            vis[u] += vis[v];
    };

    graph_orchestrator orch(V, g);

    orch.sequential_make(job);
    swap(vis, vis2);
    orch.concurrent_make(job, nthreads);

    if (vis != vis2) {
        fail("Different values");
    }
}

void graph_orchestrator_speed_run(int V, int E, int n, int nthreads) {
    vector<mat_t> vis(V), vis2(V);
    for (int i = 0; i < V; i++)
        vis[i] = vis2[i] = makemat(n, mt);

    auto g = random_exact_rooted_dag_connected(V, E);
    auto rev = make_adjacency_lists_reverse(V, g);

    auto job = [&](int u) {
        for (int v : rev[u])
            vis[u] = vis[v] * vis[u];
    };

    graph_orchestrator orch(V, g);

    START(sequential);
    orch.sequential_make(job);
    TIME(sequential);

    swap(vis, vis2);

    START(concurrent);
    orch.concurrent_make(job, nthreads);
    TIME(concurrent);

    clear_line();
    print(" V:{}, E:{}, {} threads\n", V, E, nthreads);
    PRINT_TIME(sequential);
    PRINT_TIME(concurrent);

    if (vis != vis2) {
        fail("Different values");
    }
}

void stress_test_graph_orchestrator(int T = 100) {
    int V = 4000, E = 20000;
    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test graph orchestrator");
        graph_orchestrator_stress_run(V, E, 8);
    }
}

void speed_test_graph_orchestrator(int T = 25) {
    int V = 2000, min_E = 3000, max_E = 20000;
    for (int t = 0; t < T; t++) {
        print_progress(t, T, "speed test graph orchestrator");
        int E = 1L * (t * max_E + (T - 1 - t) * min_E) / (T - 1);
        graph_orchestrator_speed_run(V, E, 50, 16);
    }
}

} // namespace stress_testing_graph_orchestrator

int main() {
    setbuf(stdout, nullptr), setbuf(stderr, nullptr);
    RUN_BLOCK(stress_test_graph_orchestrator());
    RUN_BLOCK(speed_test_graph_orchestrator());
    RUN_BLOCK(stress_test_fn_orchestrator());
    RUN_BLOCK(speed_test_fn_orchestrator());
    RUN_BLOCK(stress_test_pool_submit());
    RUN_BLOCK(stress_test_priority_pool_submit());
    return 0;
}
