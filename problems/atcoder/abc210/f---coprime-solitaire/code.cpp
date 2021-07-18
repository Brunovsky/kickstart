#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

struct twosat_scc {
    int N, C = 0; // N is the number of variables
    vector<vector<int>> adj;
    vector<int> assignment, cmap;

    explicit twosat_scc(int N = 0) : N(N), adj(2 * N) {}

    int add_var() { return adj.resize(2 * N + 2), N++; }

    void either(int u, int v) {
        u = u < 0 ? 2 * ~u : 2 * u + 1; // remove these lines if you want to input
        v = v < 0 ? 2 * ~v : 2 * v + 1; // 2u and 2u+1 directly, with 2n+1 true, 2n false
        assert(0 <= u && u < 2 * N && 0 <= v && v < 2 * N);
        adj[u ^ 1].push_back(v);
        adj[v ^ 1].push_back(u);
    }

    void at_most_one(const vector<int>& vars) {
        if (vars.size() <= 1u) {
            return;
        }
        int cur = ~vars[0];
        for (int i = 2, V = vars.size(); i < V; i++) {
            int next = add_var();
            either(cur, ~vars[i]);
            either(cur, next);
            either(~vars[i], next);
            cur = ~next;
        }
        either(cur, ~vars[1]);
    }

    void set(int u) { either(u, u); }

    vector<int> index, lowlink;
    vector<bool> onstack;
    stack<int> S;
    int depth;

    void tarjan(int u) {
        index[u] = lowlink[u] = depth++;
        S.push(u), onstack[u] = true;

        for (int v : adj[u]) {
            if (!index[v]) {
                tarjan(v);
                lowlink[u] = min(lowlink[u], lowlink[v]);
            } else if (onstack[v]) {
                lowlink[u] = min(lowlink[u], index[v]);
            }
        }

        if (index[u] == lowlink[u]) {
            int v, c = C++;
            do {
                v = S.top(), S.pop(), onstack[v] = false;
                cmap[v] = c;
                if (assignment[v >> 1] == -1)
                    assignment[v >> 1] = v & 1;
            } while (u != v);
        }
    }

    bool solve() {
        assignment.assign(N, -1);
        cmap.assign(2 * N, 0);
        index.assign(2 * N, 0);
        lowlink.assign(2 * N, 0);
        onstack.assign(2 * N, false);
        S = stack<int>();
        depth = 1, C = 0;

        for (int u = 0; u < 2 * N; u++) {
            if (!index[u])
                tarjan(u);
        }
        for (int u = 0; u < N; u++) {
            assert(assignment[u] >= 0);
            if (cmap[2 * u] == cmap[2 * u + 1])
                return false;
        }
        return true;
    }
};

vector<int> primes, lp, nxt;

auto least_prime_sieve(int N) {
    lp.assign(N + 1, 0), nxt.assign(N + 1, 0);
    nxt[1] = 1;

    for (int P = 0, n = 2; n <= N; n++) {
        if (lp[n] == 0) {
            lp[n] = n, primes.push_back(n), P++;
        }
        for (int i = 0; i < P && primes[i] <= lp[n] && n * primes[i] <= N; ++i) {
            lp[n * primes[i]] = primes[i], nxt[n * primes[i]] = n;
        }
    }
}

auto get_primes(int n) {
    vector<int> factors;
    while (n > 1) {
        int p = lp[n];
        do {
            n = nxt[n];
        } while (lp[n] == p);
        factors.push_back(p);
    }
    return factors;
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    constexpr int M = 2'000'000;
    least_prime_sieve(M);
    int P = primes.size();
    vector<int> prime_index(M);
    for (int i = 0; i < P; i++) {
        prime_index[primes[i]] = i;
    }

    int N;
    cin >> N;
    vector<int> A(N), B(N);
    for (int i = 0; i < N; i++) {
        cin >> A[i] >> B[i];
        if (A[i] == 1 || B[i] == 1) {
            A[i] = B[i] = 1;
        }
    }
    // pick ones if possible, otherwise no prime overlap

    vector<vector<int>> prime_where(P);
    vector<vector<int>> prime_lock(P);
    vector<vector<int>> A_primes(N), B_primes(N);

    for (int i = 0; i < N; i++) {
        if (A[i] == 1 || B[i] == 1) {
            continue;
        }

        A_primes[i] = get_primes(A[i]);
        B_primes[i] = get_primes(B[i]);
        debug(A_primes[i], B_primes[i]);
        for (int& a : A_primes[i]) {
            a = prime_index[a];
            prime_where[a].push_back(i);
        }
        for (int& b : B_primes[i]) {
            b = prime_index[b];
            prime_where[b].push_back(~i);
        }

        vector<int> intersection;
        set_intersection(begin(A_primes[i]), end(A_primes[i]), begin(B_primes[i]),
                         end(B_primes[i]), back_inserter(intersection));
        for (int p : intersection) {
            prime_lock[p].push_back(i);
        }
    }

    for (int p = 0; p < P; p++) {
        if (prime_lock[p].size() > 1u) {
            cout << "No\n";
            return 0;
        }
    }

    // true for B, false for A
    twosat_scc sat(N);
    for (int p = 0; p < P; p++) {
        int L = prime_lock[p].size();
        int K = prime_where[p].size();
        if (L == 0 && K > 0) {
            sat.at_most_one(prime_where[p]);
        }
        if (L == 1 && K > 0) {
            int i = prime_lock[p][0];
            for (int j : prime_where[p]) {
                if (i != j && i != ~j) {
                    sat.set(~j);
                }
            }
        }
    }

    if (sat.solve()) {
        cout << "Yes\n";
    } else {
        cout << "No\n";
    }

    return 0;
}
