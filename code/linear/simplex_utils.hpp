#ifndef SIMPLEX_UTILS_HPP
#define SIMPLEX_UTILS_HPP

#include "../formatting.hpp"
#include "simplex.hpp"

template <typename F>
bool operator==(const lp_constraint<F>& a, const lp_constraint<F>& b) {
    return a.b == b.b && a.v == b.v && a.ineq == b.ineq;
}

template <typename F>
bool operator==(const simplex<F>& a, const simplex<F>& b) {
    return a.N == b.N && a.M == b.M && a.z == b.z && a.C == b.C;
}

string to_string(LPState state) {
    static const string ss[] = {"feasible", "optimal", "unbounded", "impossible"};
    return ss[int(state)];
}

string to_string(LPIneq ineq) {
    static const string ss[] = {"<=", "==", ">="};
    return ss[int(ineq)];
}

template <typename F>
string format_formula(const vector<F>& x) {
    string s;
    for (int i = 0; i < int(x.size()); i++) {
        s += format("{}{}{}x{}", i ? " " : "", x[i] > 0 ? "+" : "", x[i], i + 1);
    }
    return s;
}

template <typename F>
string pretty_formula(const vector<F>& x) {
    string s;
    for (int i = 0; i < int(x.size()); i++) {
        s += format(" {}·x{}", x[i], i + 1);
    }
    return s;
}

template <typename F>
string format_simplex(const simplex<F>& smp) {
    int n = smp.N, m = smp.M;
    string s;
    for (int j = 0; j < n; j++) {
        s += format(" {}·x{}", smp.z[j], j + 1);
    }
    s = format("maximize {}\nsubject to\n", s);
    vector<int> width(n, 0);
    int bwidth = 0;
    for (int i = 0; i < m; i++) {
        const auto& [v, b, _] = smp.C[i];
        for (int j = 0; j < n; j++) {
            width[j] = max(width[j], int(to_string(v[j]).size()));
        }
        bwidth = max(bwidth, int(to_string(b).size()));
    }
    static const vector<string> ineq_names{"<=", "==", ">="};
    for (int i = 0; i < m; i++) {
        const auto& [v, ineq, b] = smp.C[i];
        for (int j = 0; j < n; j++) {
            s += format(" {:>{}}·x{}", v[j], width[j], j + 1);
        }
        s += format(" {} {:>{}}\n", ineq_names[int(ineq)], b, bwidth);
    }
    return s;
}

template <typename F>
string export_simplex(const simplex<F>& smp) {
    int n = smp.N, m = smp.M;
    static const auto plus = [](int i, F f) { return f < 0 ? "" : i > 0 ? "+" : " "; };
    string s;
    for (int j = 0; j < n; j++) {
        s += format(" {}{}x{}", plus(j, smp.z[j]), smp.z[j], j + 1);
    }
    s = format("maximize p = {}\nsubject to\n", s);
    vector<int> width(n, 0);
    int bwidth = 0;
    for (int i = 0; i < m; i++) {
        const auto& [v, _, b] = smp.C[i];
        for (int j = 0; j < n; j++) {
            width[j] = max(width[j], 1 + int(to_string(abs(v[j])).size()));
        }
        bwidth = max(bwidth, int(to_string(b).size()));
    }
    static const vector<string> ineq_names{"<=", "=", ">="};
    for (int i = 0; i < m; i++) {
        const auto& [v, ineq, b] = smp.C[i];
        for (int j = 0; j < n; j++) {
            auto cell = format("{}{}", plus(j, v[j]), v[j]);
            s += format(" {:>{}}x{}", cell, width[j], j + 1);
        }
        s += format(" {} {:>{}}\n", ineq_names[int(ineq)], b, bwidth);
    }
    return s;
}

template <typename F>
string format_tableau(const simplex<F>& smp) {
    int n = smp.N, m = smp.M, s = smp.S, a = smp.A;
    const auto& tab = smp.tab;
    int L = tab[0].size();
    vector<string> labels(m + 1);
    vector<string> rows(m + 1);
    vector<size_t> width(L, 0);
    size_t label_width = 0;
    string sep_row;

    auto repeat = [](int cnt, string cell) {
        string str;
        while (cnt--)
            str += cell;
        return str;
    };

    for (int i = 1; i <= m; i++) {
        int v = smp.row_var[i];
        if (1 <= v && v <= n)
            labels[i] = format("x{}", v);
        else if (n < v && v <= n + s)
            labels[i] = format("s{}", v - n);
        else if (n + s < v && v <= n + s + a)
            labels[i] = format("a{}", v - n - s);
        label_width = max(label_width, labels[i].size());
    }

    for (int i = 0; i <= m; i++) {
        for (int j = 0; j < L; j++) {
            width[j] = max(width[j], to_string(tab[i][j]).size());
        }
        rows[i] = format("{:>{}}", labels[i], label_width);
    }
    sep_row = repeat(label_width, "─");

    for (int i = 0; i <= m; i++) {
        rows[i] += format(" │ {:>{}} │", tab[i][0], width[0]);
        for (int j = 1; j <= n; j++) {
            rows[i] += format("  {:>{}}", tab[i][j], width[j]);
        }
        rows[i] += " │";
        for (int j = n + 1; j <= n + s; j++) {
            rows[i] += format("  {:>{}}", tab[i][j], width[j]);
        }
        rows[i] += " │";
        for (int j = n + s + 1; j < L; j++) {
            rows[i] += format("  {:>{}}", tab[i][j], width[j]);
        }
    }
    sep_row += "─┼" + repeat(width[0] + 1, "─") + "─┼";
    for (int j = 1; j <= n; j++) {
        sep_row += repeat(width[j] + 2, "─");
    }
    sep_row += "─┼";
    for (int j = n + 1; j <= n + s; j++) {
        sep_row += repeat(width[j] + 2, "─");
    }
    sep_row += "─┼";
    for (int j = n + s + 1; j < L; j++) {
        sep_row += repeat(width[j] + 2, "─");
    }

    string main = format("{}", fmt::join(begin(rows) + 1, end(rows), "\n"));
    return format("{}\n{}\n{}\n", rows[0], sep_row, main);
}

// return equivalent lp problem with only LP_LESS constraints
template <typename F>
auto standardize(const simplex<F>& lp) {
    simplex<F> smp;
    smp.set_objective(lp.z);
    for (int i = 0; i < lp.M; i++) {
        auto c = lp.C[i];
        if (c.ineq == LP_LESS) {
            smp.add_constraint(c);
        }
        if (c.ineq == LP_EQUAL) {
            c.ineq = LP_LESS, smp.add_constraint(c), c.ineq = LP_GREATER;
        }
        if (c.ineq == LP_GREATER) {
            c.ineq = LP_LESS;
            c.b = -c.b;
            for (auto& f : c.v)
                f = -f;
            smp.add_constraint(c);
        }
    }
    return smp;
}

// Given primal lp problem must be canonical (only LP_LESS constraints)
template <typename F>
auto make_dual(const simplex<F>& primal) {
    int n = primal.N, m = primal.M;
    simplex<F> dual;
    vector<F> z(m);
    vector<lp_constraint<F>> C(n);

    for (int i = 0; i < m; i++) {
        z[i] = -primal.C[i].b;
        assert(primal.C[i].ineq == LP_LESS);
    }
    for (int j = 0; j < n; j++) {
        C[j].v.resize(m);
        for (int i = 0; i < m; i++) {
            C[j].v[i] = -primal.C[i].v[j];
        }
        C[j].b = -primal.z[j];
        C[j].ineq = LP_LESS;
    }
    dual.set_objective(z);
    dual.add_constraints(C);
    return dual;
}

template <typename F>
bool is_feasible(const lp_constraint<F>& c, const vector<F>& x) {
    assert(c.v.size() == x.size());
    F a = 0;
    for (int i = 0; i < int(x.size()); i++) {
        a += c.v[i] * x[i];
    }
    return (c.ineq == LP_LESS && a <= c.b) || (c.ineq == LP_EQUAL && a == c.b) ||
           (c.ineq == LP_GREATER && a >= c.b);
}

template <typename F>
bool is_feasible(const simplex<F>& lp, const vector<F>& x) {
    return all_of(begin(x), end(x), [](F f) { return f >= 0; }) &&
           all_of(begin(lp.C), end(lp.C),
                  [&x](const lp_constraint<F>& c) { return is_feasible(c, x); });
}

#endif // SIMPLEX_UTILS_HPP
