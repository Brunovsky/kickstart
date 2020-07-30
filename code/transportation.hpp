#include <bits/stdc++.h>

using namespace std;

// *****

struct transportation {
    vector<int> supply;
    vector<int> demand;
    vector<vector<int>> cost;

    // fill in the matrices...

    int n, m;
    vector<int> u;
    vector<int> v;
    vector<set<int>> row_basis;
    vector<set<int>> col_basis;
    vector<bool> row_vis;
    vector<bool> col_vis;
    vector<vector<int>> tp; // transport
    enum State { FOUND = 0, OPTIMAL = 1 };
    static constexpr int nil = INT_MIN;

    void potential_dfs_row(int r) {
        assert(u[r] != nil);
        for (int c : row_basis[r]) {
            if (v[c] == nil) {
                v[c] = cost[r][c] - u[r];
                potential_dfs_col(c);
            }
        }
    }

    void potential_dfs_col(int c) {
        assert(v[c] != nil);
        for (int r : col_basis[c]) {
            if (u[r] == nil) {
                u[r] = cost[r][c] - v[c];
                potential_dfs_row(r);
            }
        }
    }

    State select_pivot(int& r, int& c) {
        fill(begin(u), end(u), nil);
        fill(begin(v), end(v), nil);
        int x = 0, y = *row_basis[0].begin();
        u[x] = 0, v[y] = cost[x][y];
        potential_dfs_row(x), potential_dfs_col(y);

        int delta = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (tp[i][j] == 0 && delta + u[i] + v[j] > cost[i][j]) {
                    delta = cost[i][j] - u[i] - v[j];
                    r = i, c = j;
                }
            }
        }
        return delta < 0 ? FOUND : OPTIMAL;
    }

    int pivot_dfs_row(int r, int y, int prev_boost) {
        if (row_vis[r]) {
            return -1;
        }
        row_vis[r] = true;
        for (int c : row_basis[r]) {
            int boost = pivot_dfs_col(c, y, min(prev_boost, tp[r][c]));
            if (boost != -1) {
                tp[r][c] -= boost;
                if (tp[r][c] == 0 && boost != prev_boost) {
                    row_basis[r].erase(c);
                    col_basis[c].erase(r);
                }
                return boost;
            }
        }
        return -1;
    }

    int pivot_dfs_col(int c, int y, int prev_boost) {
        if (c == y) {
            return prev_boost;
        }
        if (col_vis[c]) {
            return -1;
        }
        col_vis[c] = true;
        for (int r : col_basis[c]) {
            int boost = pivot_dfs_row(r, y, prev_boost);
            if (boost != -1) {
                tp[r][c] += boost;
                return boost;
            }
        }
        return -1;
    }

    void pivot(int r, int c) {
        assert(tp[r][c] == 0);
        row_vis.assign(n, false);
        col_vis.assign(m, false);
        int boost = pivot_dfs_row(r, c, INT_MAX);
        assert(boost >= 0);
        tp[r][c] = boost;
        row_basis[r].insert(c);
        col_basis[c].insert(r);
    }

    void compute() {
        vector<int> rem_supply = supply;
        vector<int> rem_demand = demand;

        int tsupply = accumulate(begin(supply), end(supply), 0);
        int tdemand = accumulate(begin(demand), end(demand), 0);

        if (tsupply < tdemand) {
            rem_supply.push_back(tdemand - tsupply);
            cost.push_back(vector<int>(rem_demand.size(), 0));
        } else if (tsupply > tdemand) {
            rem_demand.push_back(tsupply - tdemand);
            for (auto& row : cost) {
                row.push_back(0);
            }
        }

        n = rem_supply.size();
        m = rem_demand.size();
        u.assign(n, nil);
        v.assign(m, nil);
        row_vis.resize(n);
        col_vis.resize(m);
        row_basis.assign(n, {});
        col_basis.assign(m, {});
        tp.assign(n, vector<int>(m, 0));

        // find basic feasible solution
        int r = 0, c = 0;
        while (r < n && c < m) {
            row_basis[r].insert(c);
            col_basis[c].insert(r);
            tp[r][c] = min(rem_supply[r], rem_demand[c]);
            rem_supply[r] -= tp[r][c];
            rem_demand[c] -= tp[r][c];
            r += rem_demand[c] && !rem_supply[r];
            c += !rem_demand[c];
        }

        // improvement loop
        while (FOUND == select_pivot(r, c)) {
            pivot(r, c);
        }

        if (tsupply < tdemand) {
            tp.pop_back();
            cost.pop_back();
        } else if (tsupply > tdemand) {
            for (int i = 0; i < n; i++) {
                tp[i].pop_back();
                cost[i].pop_back();
            }
        }
    }
};
constexpr int transportation::nil;

template <typename N>
string to_string(const vector<vector<N>>& tab) {
    vector<size_t> width(tab[0].size());
    for (uint i = 0; i < tab.size(); i++) {
        for (uint j = 0; j < tab[i].size(); j++) {
            width[j] = max(width[j], 1 + to_string(tab[i][j]).length());
        }
    }
    stringstream ss;
    for (uint i = 0; i < tab.size(); i++) {
        for (uint j = 0; j < tab[i].size(); j++) {
            ss << setw(width[j]) << to_string(tab[i][j]) << ' ';
        }
        ss << '\n';
    }
    return ss.str();
}
