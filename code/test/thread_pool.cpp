#include "../thread_pool.hpp"

#include "../bits.hpp"
#include "../debug_print.hpp"
#include "../graph_generator.hpp"
#include "../matrix.hpp"
#include "../random.hpp"
#include "chrono.hpp"

// *****

using mat_t = mat<long>;

mat_t makemat(int n, default_random_engine& rnd) {
    longd distv(-100, 100);
    mat_t mat(n, n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[i][j] = distv(rnd);
    return mat;
}

void compute(int id) {
    intd distn(200, 500);
    default_random_engine rnd(random_device{}());
    int n = distn(rnd);
    auto a = makemat(n, rnd);
    auto b = makemat(n, rnd);
    auto c = a * b;
    char buf[300]{};
    sprintf(buf, " %4d product complete, n: %3d", id, n);
    puts(buf);
}

void compute_priority(int id, int priority) {
    intd distn(200, 500);
    default_random_engine rnd(random_device{}());
    int n = distn(rnd);
    auto a = makemat(n, rnd);
    auto b = makemat(n, rnd);
    auto c = a * b;
    char buf[300]{};
    sprintf(buf, " %4d product complete, n: %3d, p: %4d", id, n, priority);
    puts(buf);
}

template <typename T>
struct action_t {
    int b1, b2;
    T value;
};

void test_iterative_add(int N = 200, int rate = 47, int P = 5) {
    thread_pool pool(P);
    assert(pool.pool_size() == P);
    int i = 1;
    while (i <= N) {
        int j = 0;
        while (i <= N && j < rate) {
            pool.submit(compute, i), i++, j++;
        }
        char buf[300]{};
        sprintf(buf, "-- %4d..%4d, added %4d jobs", i - j, i - 1, j);
        puts(buf);
        if (j == rate) {
            pool.wait();
        } else {
            pool.cancel();
        }
    }
}

void test_iterative_add_priority(int N = 200, int rate = 47, int P = 5) {
    priority_thread_pool<int> pool(P);
    assert(pool.pool_size() == P);
    int i = 1;
    intd priorityd(1000, 9999);
    while (i <= N) {
        int j = 0;
        while (i <= N && j < rate) {
            int p = priorityd(mt);
            pool.submit(p, compute_priority, i, p), i++, j++;
        }
        char buf[300]{};
        sprintf(buf, "-- %4d..%4d, added %4d jobs", i - j, i - 1, j);
        puts(buf);
        if (j == rate) {
            pool.wait();
        } else {
            pool.cancel();
        }
    }
}

void fn_orchestrator_unit_test(int V, int buckets, int nthreads) {
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

    orch.sequential_make(job);
    swap(vis, vis2);

    orch.concurrent_make(job, nthreads);

    int S = 0;
    for (int b = 0; b < buckets; b++)
        S += vis[b].size();

    if (vis != vis2 || S != 2 * V) {
        print("\n");
        if (vis != vis2)
            print("  -- error: different visitors\n");
        if (S != 2 * V)
            print("  -- error: bad count: {}/{}\n", S, 2 * V);
    }
}

void fn_orchestrator_unit_speed_test(int V, int n, int buckets, int nthreads) {
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
        print("\n  -- error: different matrices\n");
    }
}

void test_fn_orchestrator(int T = 100) {
    for (int t = 1; t <= T; t++) {
        print("\rtest orchestrator {}...", t);
        fn_orchestrator_unit_test(600, 40, 32);
    }
    print("\n");
}

void test_speed_fn_orchestrator(int T = 25) {
    for (int t = 1; t <= T; t++) {
        print("\rtest orchestrator {}...", t);
        fn_orchestrator_unit_speed_test(1000, 70, 3 * t, 16);
    }
    print("\n");
}

void graph_orchestrator_unit_test(int V, int E, int nthreads) {
    vector<unsigned long> vis(V, 1), vis2(V, 1);

    auto g = random_exact_rooted_dag_connected(V, E);
    auto rev = make_adjacency_lists_reverse(g, V);

    auto job = [&](int u) {
        for (int v : rev[u])
            vis[u] += vis[v];
    };

    graph_orchestrator orch(V, g);

    orch.sequential_make(job);
    swap(vis, vis2);
    orch.concurrent_make(job, nthreads);

    if (vis != vis2) {
        print("\n  -- error: different values\n");
    }
}

void graph_orchestrator_unit_speed_test(int V, int E, int n, int nthreads) {
    vector<mat_t> vis(V), vis2(V);
    for (int i = 0; i < V; i++)
        vis[i] = vis2[i] = makemat(n, mt);

    auto g = random_exact_rooted_dag_connected(V, E);
    auto rev = make_adjacency_lists_reverse(g, V);

    auto job = [&](int u) {
        for (int v : rev[u])
            vis[u] = vis[v] * vis[u];
    };

    graph_orchestrator orch(V, g);

    print("\r V:{}, E:{}, {} threads\n", V, E, nthreads);

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
        print("\n  -- error: different values\n");
    }
}

void test_graph_orchestrator(int T = 100) {
    int V = 4000, E = 20000;
    for (int t = 1; t <= T; t++) {
        print("\rtest orchestrator {}...", t);
        graph_orchestrator_unit_test(V, E, 8);
    }
    print("\n");
}

void test_speed_graph_orchestrator(int T = 25) {
    int V = 2000, min_E = 3000, max_E = 20000;
    for (int t = 1; t <= T; t++) {
        print("\rtest orchestrator {}...", t);
        int E = 1L * (t * max_E + (T - t) * min_E) / T;
        graph_orchestrator_unit_speed_test(V, E, 50, 16);
    }
    print("\n");
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_graph_orchestrator();
    test_speed_graph_orchestrator();
    test_fn_orchestrator();
    test_speed_fn_orchestrator();
    test_iterative_add();
    test_iterative_add_priority();
    return 0;
}
