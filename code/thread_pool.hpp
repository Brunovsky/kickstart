#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include "graph.hpp"
#include "integer_lists.hpp"

// *****

/**
 * Thread pool that can be used by multiple threads; jobs processed in FIFO order.
 */
struct thread_pool {
  private:
    using job_t = function<void()>;
    using queue_t = queue<job_t, list<job_t>>;
    enum pool_status : uint8_t { ready, cancelled, invalid };

    vector<thread> threads;
    queue_t jobs;
    mutable mutex mtx;
    condition_variable cv, cv_user;
    pool_status state = ready;
    uint idle = 0, waiting = 0;

    inline int pending_unsafe() const { return jobs.size() + pool_size() - idle; }
    inline int worker_ready() const { return !jobs.empty() || state == cancelled; }

    void init_thread() {
        threads.emplace_back([this]() {
            while (true) {
                unique_lock guard(mtx);
                ++idle;
                if (state == cancelled) {
                    break;
                }
                if (waiting) {
                    cv_user.notify_all();
                }
                if (jobs.empty()) {
                    cv.wait(guard, [this]() { return worker_ready(); });
                }
                if (state == cancelled) {
                    break;
                }
                --idle;
                auto job = move(jobs.front());
                jobs.pop();
                guard.unlock();
                job();
            }
        });
    }

  public:
    explicit thread_pool(int nthreads) {
        assert(nthreads > 0);
        threads.reserve(nthreads);
        for (int id = 0; id < nthreads; id++)
            init_thread();
    }

    ~thread_pool() noexcept { cancel(); }

    /**
     * Submit a job to be run.
     * The function must be callable with the provided arguments.
     * Be careful of argument scope.
     */
    template <typename Fn, typename... Args>
    void submit(Fn&& fn, Args&&... args) {
        job_t job = bind(forward<Fn>(fn), forward<Args>(args)...);
        lock_guard guard(mtx);
        assert(state == ready);
        jobs.emplace(move(job));
        cv.notify_one();
    }

    /**
     * Block until all jobs have been executed.
     * Afterwards the pool is empty and valid, and more jobs can be added still.
     */
    void wait() {
        unique_lock guard(mtx);
        if (pending_unsafe() == 0)
            return;

        waiting++;
        cv_user.wait(guard, [this]() { return pending_unsafe() == 0; });
        waiting--;
    }

    /**
     * Blocks until k more jobs have finished running.
     * Afterwards the pool is valid, and more jobs can be added still.
     */
    void wait_for(int k) {
        unique_lock guard(mtx);
        if (pending_unsafe() == 0)
            return;

        k = max(0, pending_unsafe() - k);
        waiting++;
        cv_user.wait(guard, [this, k]() { return pending_unsafe() <= k; });
        waiting--;
    }

    /**
     * Blocks until there are only k jobs running or pending.
     * If there are already <=k jobs running or pending, wait for 1 job to finish.
     * Afterwards the pool is valid, and more jobs can be added still.
     */
    void wait_until(int k) {
        unique_lock guard(mtx);
        if (pending_unsafe() == 0)
            return;

        k = max(0, min(pending_unsafe() - 1, k));
        waiting++;
        cv_user.wait(guard, [this, k]() { return pending_unsafe() <= k; });
        waiting--;
    }

    /**
     * Block until all jobs have been executed and join with all threads.
     * Afterwards, the pool is empty and invalid.
     */
    void finish() noexcept {
        unique_lock guard(mtx);
        if (state == invalid)
            return;

        waiting++;
        cv_user.wait(guard, [this]() { return pending_unsafe() == 0; });
        waiting--;

        state = cancelled;
        cv.notify_all();
        guard.unlock();
        for (thread& worker : threads)
            assert(worker.joinable()), worker.join();

        state = invalid;
    }

    /**
     * Block until all running jobs have been executed and join with all threads.
     * Jobs pending in the job queue are not executed.
     * Afterwards, the pool is empty and invalid.
     */
    void cancel() noexcept {
        unique_lock guard(mtx);
        if (state == invalid)
            return;

        state = cancelled;
        cv.notify_all();
        guard.unlock();
        for (thread& worker : threads)
            assert(worker.joinable()), worker.join();

        state = invalid;
    }

    inline int pending() const noexcept {
        lock_guard guard(mtx);
        return pending_unsafe();
    }
    inline int pool_size() const noexcept { return threads.size(); }
    inline bool empty() const noexcept { return pending() == 0; }
};

/**
 * Thread pool designed for a unique owner, jobs processed by priority order.
 * Pretty much identical to the basic thread pool.
 */
template <typename P, typename Compare = less<>>
struct priority_thread_pool {
  private:
    using job_t = function<void()>;
    using entry_t = pair<P, job_t>;
    struct ComparePriority : Compare {
        ComparePriority(const Compare& comp) : Compare(comp) {}
        inline bool operator()(const entry_t& a, const entry_t& b) noexcept {
            return Compare::operator()(a.first, b.first);
        }
    };
    using queue_t = priority_queue<entry_t, vector<entry_t>, ComparePriority>;
    enum pool_status : uint8_t { ready, cancelled, invalid };

    vector<thread> threads;
    queue_t jobs;
    mutable mutex mtx;
    condition_variable cv, cv_user;
    pool_status state = ready;
    uint idle = 0, waiting = 0;

    inline int pending_unsafe() const { return jobs.size() + pool_size() - idle; }
    inline int worker_ready() const { return !jobs.empty() || state == cancelled; }

    void init_thread() {
        threads.emplace_back([this]() {
            while (true) {
                unique_lock guard(mtx);
                ++idle;
                if (state == cancelled) {
                    break;
                }
                if (waiting) {
                    cv_user.notify_all();
                }
                if (jobs.empty()) {
                    cv.wait(guard, [this]() { return worker_ready(); });
                }
                if (state == cancelled) {
                    break;
                }
                --idle;
                auto job = move(jobs.top().second);
                jobs.pop();
                guard.unlock();
                job();
            }
        });
    }

  public:
    explicit priority_thread_pool(int nthreads, const Compare& comp = Compare())
        : jobs(comp) {
        assert(nthreads > 0);
        threads.reserve(nthreads);
        for (int id = 0; id < nthreads; id++)
            init_thread();
    }

    ~priority_thread_pool() noexcept { cancel(); }

    /**
     * Submit a job to be run.
     * The function must be callable with the provided arguments.
     * Be careful of argument scope.
     */
    template <typename Fn, typename... Args>
    void submit(P priority, Fn&& fn, Args&&... args) {
        job_t job = bind(forward<Fn>(fn), forward<Args>(args)...);
        lock_guard guard(mtx);
        assert(state == ready);
        jobs.emplace(move(priority), move(job));
        cv.notify_one();
    }

    /**
     * Block until all jobs have been executed.
     * Afterwards the pool is empty and valid, and more jobs can be added.
     */
    void wait() noexcept {
        unique_lock guard(mtx);
        if (pending_unsafe() == 0)
            return;

        waiting++;
        cv_user.wait(guard, [this]() { return pending_unsafe() == 0; });
        waiting--;
    }

    /**
     * Blocks until k more jobs have finished running.
     * Afterwards the pool is valid, and more jobs can be added still.
     */
    void wait_for(int k) {
        unique_lock guard(mtx);
        if (pending_unsafe() == 0)
            return;

        k = max(0, pending_unsafe() - k);
        waiting++;
        cv_user.wait(guard, [this, k]() { return pending_unsafe() <= k; });
        waiting--;
    }

    /**
     * Blocks until there are only k jobs running or pending.
     * If there are already <=k jobs running or pending, wait for 1 job to finish.
     * Afterwards the pool is valid, and more jobs can be added still.
     */
    void wait_until(int k) {
        unique_lock guard(mtx);
        if (pending_unsafe() == 0)
            return;

        k = max(0, min(pending_unsafe() - 1, k));
        waiting++;
        cv_user.wait(guard, [this, k]() { return pending_unsafe() <= k; });
        waiting--;
    }

    /**
     * Block until all jobs have been executed and join with all threads.
     * Afterwards, the pool is empty and invalid.
     */
    void finish() noexcept {
        unique_lock guard(mtx);
        if (state == invalid)
            return;

        waiting++;
        cv_user.wait(guard, [this]() { return pending_unsafe() == 0; });
        waiting--;

        state = cancelled;
        cv.notify_all();
        guard.unlock();
        for (thread& worker : threads)
            assert(worker.joinable()), worker.join();

        state = invalid;
    }

    /**
     * Block until all running jobs have been executed and join with all threads.
     * Jobs pending in the job queue are not executed.
     * Afterwards, the pool is empty and invalid.
     */
    void cancel() {
        unique_lock guard(mtx);
        if (state == invalid)
            return;

        state = cancelled;
        cv.notify_all();
        guard.unlock();
        for (thread& worker : threads)
            assert(worker.joinable()), worker.join();

        state = invalid;
    }

    inline uint pending() const noexcept {
        lock_guard guard(mtx);
        return jobs.size() + pool_size() - idle;
    }
    inline uint pool_size() const noexcept { return threads.size(); }
    inline bool empty() const noexcept { return pending() == 0; }
};

/**
 * A spinlock class that yields after N attempts to lock the spinlock.
 */
template <unsigned N = 16>
class alignas(64) spinlock {
    atomic_flag flag = ATOMIC_FLAG_INIT;

  public:
    spinlock() = default;
    spinlock(const spinlock&) = delete;
    spinlock& operator=(const spinlock&) = delete;

    void lock() noexcept {
        uint16_t count = 0;
        while (flag.test_and_set(std::memory_order_acquire)) {
            if (++count == N) {
                this_thread::yield();
                count = 0;
            }
        }
    }

    void unlock() noexcept { flag.clear(std::memory_order_release); }

    bool try_lock() noexcept {
        uint16_t count = 0;
        while (flag.test_and_set(std::memory_order_acquire)) {
            if (++count == N)
                return false;
        }
        return true;
    }
};

/**
 * A shared mutex based on the spinlock implementation above.
 */
template <unsigned N = 16>
class shared_spinlock {
    spinlock<N> mutex;
    condition_variable_any cv_shared, cv_unique;
    uint16_t state;

    static constexpr uint16_t unique_mask = 1u << 15u;
    static constexpr uint16_t max_readers = uint16_t(~unique_mask);

    bool unique_request() const noexcept { return state & unique_mask; }
    bool unique_mode() const noexcept { return state == unique_mask; }
    uint16_t readers() const noexcept { return state & max_readers; }

  public:
    shared_spinlock() = default;
    shared_spinlock(const shared_spinlock&) = delete;
    shared_spinlock& operator=(const shared_spinlock&) = delete;

    // Writers

    void lock() noexcept {
        unique_lock lock(mutex);
        cv_shared.wait(lock, [&] { return !unique_request(); });
        state |= unique_mask;
        cv_unique.wait(lock, [&] { return !readers(); });
    }

    bool try_lock() {
        unique_lock lock(mutex, std::try_to_lock);
        if (lock.owns_lock() && state == 0) {
            state = unique_mask;
            return true;
        }
        return false;
    }

    void unlock() {
        lock_guard lock(mutex);
        assert(unique_mode());
        state = 0;
        cv_shared.notify_all();
    }

    // Readers

    void lock_shared() {
        unique_lock lock(mutex);
        if (state >= max_readers)
            cv_shared.wait(lock, [&] { return state < max_readers; });
        ++state;
    }

    bool try_lock_shared() {
        unique_lock lock(mutex, std::try_to_lock);
        if (lock.owns_lock() && state < max_readers) {
            ++state;
            return true;
        }
        return false;
    }

    void unlock_shared() {
        lock_guard lock(mutex);
        assert(readers() > 0);
        --state;
        if (unique_mode())
            cv_unique.notify_one();
    }
};

template <typename T>
struct concurrent_queue {
  private:
    vector<T> buf;
    uint l = 0, r = 0;
    mutable spinlock<8 * sizeof(T)> mtx;

  public:
    explicit concurrent_queue(uint N) : buf(N) {}
    concurrent_queue(const concurrent_queue&) = delete;
    concurrent_queue& operator=(const concurrent_queue&) = delete;

    uint size() const {
        lock_guard guard(mtx);
        return r - l;
    }
    bool empty() const {
        lock_guard guard(mtx);
        return l == r;
    }
    void reset() { l = r = 0; }

    void push(const T& val) {
        lock_guard guard(mtx);
        assert(r < buf.size());
        buf[r++] = val;
    }
    const T& pop() {
        lock_guard guard(mtx);
        assert(l < r);
        return buf[l++];
    }
    bool maybe_pop(T& elem) {
        lock_guard guard(mtx);
        return l < r ? elem = buf[l++], true : false;
    }
};

/**
 * An orchestrator is like classic unix make.
 *
 * There is a (possibly implicit) graph made up of N nodes.
 * For each node u, we want to run some work, job(u).
 *
 * Between the nodes are dependency relations: for nodes u,v, it may be required that
 * job(u) be processed before job(v). In a directed graph this is analogous to an edge
 * from u to v. To satisfy the dependency relation requirements, the jobs must be
 * processed in topological order.
 *
 * However, we would like to run the jobs in parallel if possible, so while running
 * the jobs one by one in topological order is sufficient to guarantee correctness, it
 * is usually excessive as there won't be so many interdependencies: in most use
 * cases, neither u will depend on v nor v depend on u, so they can be processed in
 * parallel if all their dependencies have been processed.
 *
 * This is the practical solution to the job-scheduling problem, where we don't wan't
 * to optimize anything (like makespan or cost), we just want to run the damn jobs.
 *
 * For simplicity the orchestrator operates over the integers [0...N).
 */

/**
 * The first orchestrator version uses an implicit graph and is governed by a
 * black-box dependency function D. The function D is such that D(u,v) is true if and
 * only if v depends on u (that is, u must be processed before v). We assume that the
 * ordering of nodes 0...N-1 is a valid topological order of the nodes. The function D
 * does not need to be a valid comparison function; in particular, it does not need to
 * be transitive nor total.
 *
 * Since D is a black-box the main thread needs to use heuristics to govern the amount
 * of time it spends looking for new runnable jobs and the amount of time it spends
 * waiting for the current jobs to finish running, and therefore allow earlier jobs to
 * be started. I don't see an easy solution to this - searching for too long to
 * collect dependencies might mean we take too long processing a finished job;
 * searching too little might mean we miss runnable jobs.
 *
 * In this implementation, the main thread is greedy and will search for runnable jobs
 * until the memory requirements of the dependency tracking exceed 2N + T^2 integers,
 * where T is the number of threads. Also, the concurrent queue makes it so that no real
 * synchronization is needed between the runners and the main thread.
 * If the jobs don't run too quickly, this requires O(N + T^2) memory and at any point
 * guarantees at least O(N^1/2) of the pending jobs have their dependencies completely
 * determined. Also, the nodes are processed by priority, which is their number of
 * dependents at the time of insertion.
 */
template <typename Depends>
struct fn_orchestrator {
  private:
    int N;
    Depends d;

  public:
    explicit fn_orchestrator(int N, const Depends& d = Depends()) : N(N), d(d) {}

    bool verify() const {
        for (int u = 1; u < N; u++)
            if (d(u, u - 1))
                return false;
        return true;
    }

    template <typename Fn>
    void sequential_make(const Fn& job) {
        for (int u = 0; u < N; u++)
            job(u);
    }

    template <typename Fn>
    void concurrent_make(const Fn& job, int nthreads) {
        vector<int> cnt(N, -1);
        vector<vector<int>> dependents(N);
        linked_lists open(1, N); // 1 list only
        priority_thread_pool<int> pool(nthreads);
        concurrent_queue<int> done(N);
        unsigned long heavy = 0;
        const unsigned long maxmem = 1L * nthreads * nthreads + 2L * N;

        auto runner = [&job, &done](int u) { job(u), done.push(u); };

        for (int u = 0; u < N; u++) {
            open.push_back(0, u);
        }

        for (int v = 0; v < N; v++) {
            if (heavy >= maxmem) {
                auto needed = max(1ul, (heavy - maxmem) / N);
                pool.wait_for(needed);
            }
            while (!done.empty()) {
                int u = done.pop();
                for (int w : dependents[u]) {
                    if (--cnt[w] == 0) {
                        int priority = dependents[w].size();
                        pool.submit(priority, runner, w);
                    }
                }
                heavy -= dependents[u].size();
                vector<int> empty;
                swap(dependents[u], empty);
                open.erase(u);
            }
            cnt[v] = 0;
            FOR_EACH_IN_LINKED_LIST (u, 0, open) {
                if (u == v)
                    break;
                if (d(u, v)) {
                    dependents[u].push_back(v);
                    cnt[v]++;
                }
            }
            heavy += cnt[v];
            if (cnt[v] == 0) {
                pool.submit(-1, runner, v);
            }
        }

        while (heavy > 0) {
            pool.wait_for(1);
            while (!done.empty()) {
                int u = done.pop();
                for (int w : dependents[u]) {
                    if (--cnt[w] == 0) {
                        int priority = dependents[w].size();
                        pool.submit(priority, runner, w);
                    }
                }
                heavy -= dependents[u].size();
                vector<int> empty;
                swap(dependents[u], empty);
                open.erase(u);
            }
        }

        pool.finish();
    }
};

/**
 * The second orchestrator version uses an explicit graph. The amount of work done in
 * the the main thread is O(E) locks/unlocks, where E is the number of edges in the
 * graph. Cycles are allowed to exist: nodes in cycles are simply ignored.
 */
struct graph_orchestrator {
  private:
    int N, E;
    vector<int> adj, off, deps;

    auto toposort() const {
        vector<int> cnt(N, 0), dfs;
        int j = 0, S = 0;

        for (int u = 0; u < N; u++)
            if (deps[u] == 0)
                dfs.push_back(u), S++;

        while (j < S) {
            int u = dfs[j++];
            for (int i = off[u]; i < off[u + 1]; i++)
                if (int v = adj[i]; ++cnt[v] == deps[v])
                    dfs.push_back(v), S++;
        }

        return dfs;
    }

  public:
    explicit graph_orchestrator(int N, const edges_t& g)
        : N(N), E(g.size()), adj(E), off(N + 1, 0), deps(N, 0) {
        for (auto [u, v] : g)
            off[u + 1]++, deps[v]++;
        partial_sum(begin(off), end(off), begin(off));
        auto cur = off;
        for (auto [u, v] : g)
            adj[cur[u]++] = v;
    }

    bool verify() const { return toposort().size() == N; }

    template <typename Fn>
    void sequential_make(const Fn& job) {
        for (int u : toposort())
            job(u);
    }

    template <typename Fn>
    void concurrent_make(const Fn& job, int nthreads) {
        vector<int> cnt(N, 0);
        priority_thread_pool<int> pool(nthreads);
        concurrent_queue<int> done(N);
        int seen = 0;

        auto runner = [&job, &done](int u) { job(u), done.push(u); };

        for (int u = 0; u < N; u++)
            if (deps[u] == 0)
                pool.submit(off[u + 1] - off[u], runner, u), seen++;

        while (seen < N) {
            pool.wait_for(1);
            while (!done.empty()) {
                int u = done.pop();
                for (int i = off[u]; i < off[u + 1]; i++) {
                    int v = adj[i];
                    if (++cnt[v] == deps[v])
                        pool.submit(off[v + 1] - off[v], runner, v), seen++;
                }
            }
        }

        pool.finish();
    }
};

#endif // THREAD_POOL_HPP
