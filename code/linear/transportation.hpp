#ifndef TRANSPORTATION_HPP
#define TRANSPORTATION_HPP

#include "matrix.hpp"

struct transportation {
    int N, M;
    array<vector<int>, 2> flow;
    mat<int> cost, tp;
    vector<int> upi, vpi, uQ, vQ, uvis, vvis;
    vector<vector<int>> ubasis, vbasis;
    int iteration;

    static inline const int nil = INT_MIN, inf = INT_MAX;

    transportation(array<vector<int>, 2> flow, mat<int> cost)
        : N(flow[0].size()), M(flow[1].size()), flow(move(flow)), cost(move(cost)) {
        assert(N > 0 && M > 0);
    }

    void add_basis(int u, int v) { ubasis[u].push_back(v), vbasis[v].push_back(u); }
    void rem_basis(int u, int v) {
        remove(begin(ubasis[u]), end(ubasis[u]), v), ubasis[u].pop_back();
        remove(begin(vbasis[v]), end(vbasis[v]), u), vbasis[v].pop_back();
    }

    void init_feasible() {
        int u = 0, v = 0;
        while (u < N && v < M) {
            add_basis(u, v);
            tp[u][v] = min(flow[0][u], flow[1][v]);
            flow[0][u] -= tp[u][v], flow[1][v] -= tp[u][v];
            u += !flow[0][u] && flow[1][v], v += !flow[1][v];
        }
    }

    void adjust_potentials() {
        int i = 0, j = 0, U = 0, V = 0;
        uvis[0] = ++iteration, upi[0] = 0, uQ[U++] = 0;

        do {
            while (i < U) {
                int u = uQ[i++];
                for (int v : ubasis[u]) {
                    if (vvis[v] < iteration) {
                        vvis[v] = iteration, vpi[v] = cost[u][v] - upi[u], vQ[V++] = v;
                    }
                }
            }
            while (j < V) {
                int v = vQ[j++];
                for (int u : vbasis[v]) {
                    if (uvis[u] < iteration) {
                        uvis[u] = iteration, upi[u] = cost[u][v] - vpi[v], uQ[U++] = u;
                    }
                }
            }
        } while (i < U || j < V);
        assert(U == N && V == M);

        for (int u = 0; u < N; u++)
            for (int v : ubasis[u])
                assert(cost[u][v] == upi[u] + vpi[v]);
    }

    tuple<int, int, int> select_pivot() const {
        int delta = 0, u = 0, v = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                if (tp[i][j] == 0 && delta + upi[i] + vpi[j] > cost[i][j]) {
                    delta = cost[i][j] - upi[i] - vpi[j];
                    u = i, v = j;
                }
            }
        }
        return {delta, u, v};
    }

    int pivot_row(int u, int y, int predf) {
        uvis[u] = iteration;
        for (int v : ubasis[u]) {
            if (vvis[v] < iteration) {
                int df = v == y ? min(predf, tp[u][v])
                                : pivot_col(v, y, min(predf, tp[u][v]));
                if (df != -1) {
                    tp[u][v] -= df;
                    if (tp[u][v] == 0 && df != predf) {
                        rem_basis(u, v);
                    }
                    return df;
                }
            }
        }
        return -1;
    }

    int pivot_col(int v, int y, int predf) {
        vvis[v] = iteration;
        for (int u : vbasis[v]) {
            if (uvis[u] < iteration) {
                int df = pivot_row(u, y, predf);
                if (df != -1) {
                    tp[u][v] += df;
                    return df;
                }
            }
        }
        return -1;
    }

    int pivot(int u, int v) {
        iteration++;
        int df = pivot_row(u, v, inf);
        tp[u][v] = df;
        add_basis(u, v);
        return df;
    }

    long compute() {
        int flow0 = accumulate(begin(flow[0]), end(flow[0]), 0);
        int flow1 = accumulate(begin(flow[1]), end(flow[1]), 0);

        if (flow0 < flow1) {
            flow[0].push_back(flow1 - flow0), N++, cost.resize(N, M, 0);
        } else if (flow0 > flow1) {
            flow[1].push_back(flow0 - flow1), M++, cost.resize(N, M, 0);
        }

        upi.resize(N), vpi.resize(M);
        uQ.resize(N), vQ.resize(M);
        uvis.assign(N, 0), vvis.assign(M, 0);
        ubasis.assign(N, {}), vbasis.assign(M, {});
        tp.assign(N, M, 0);
        iteration = 0;

        init_feasible();

        do {
            adjust_potentials();
            auto [delta, u, v] = select_pivot();
            if (delta >= 0)
                break;
            pivot(u, v);
        } while (true);

        if (flow0 < flow1) {
            N--, tp.resize(N, M);
        } else if (flow0 > flow1) {
            M--, tp.resize(N, M);
        }

        long ans = 0;
        for (int u = 0; u < N; u++)
            for (int v : ubasis[u])
                ans += 1L * tp[u][v] * cost[u][v];
        return ans;
    }

    static long solve(const array<vector<int>, 2>& flow, const mat<int>& cost,
                      mat<int>* out_tp = nullptr) {
        transportation solver(flow, cost);
        long ans = solver.compute();
        !out_tp || (*out_tp = move(solver.tp), 0);
        return ans;
    }
};

#endif // TRANSPORTATION_HPP
