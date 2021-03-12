#ifndef SIMPLEX_UTILS_HPP
#define SIMPLEX_UTILS_HPP

#include "simplex.hpp"

// *****

string format_formula(const vector<frac>& x) {
    string s;
    for (int i = 0; i < x.size(); i++) {
        s += format("{}{}{}x{}", i ? " " : "", x[i] > 0 ? "+" : "", x[i], i + 1);
    }
    return s;
}

string pretty_formula(const vector<frac>& x) {
    string s;
    for (int i = 0; i < x.size(); i++) {
        s += format(" {}·x{}", x[i], i + 1);
    }
    return s;
}

string format_simplex(const simplex& smp) {
    int n = smp.n, m = smp.m;
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
    static const vector<string> ineq{"<=", "==", ">="};
    for (int i = 0; i < m; i++) {
        const auto& [v, b, type] = smp.C[i];
        for (int j = 0; j < n; j++) {
            s += format(" {:>{}}·x{}", v[j], width[j], j + 1);
        }
        s += format(" {} {:>{}}\n", ineq[int(type)], b, bwidth);
    }
    return s;
}

string export_simplex(const simplex& smp) {
    int n = smp.n, m = smp.m;
    static auto plus = [](int i, frac f) { return f < 0 ? "" : i > 0 ? "+" : " "; };
    string s;
    for (int j = 0; j < n; j++) {
        s += format(" {}{}x{}", plus(j, smp.z[j]), smp.z[j], j + 1);
    }
    s = format("maximize p = {}\nsubject to\n", s);
    vector<int> width(n, 0);
    int bwidth = 0;
    for (int i = 0; i < m; i++) {
        const auto& [v, b, _] = smp.C[i];
        for (int j = 0; j < n; j++) {
            width[j] = max(width[j], 1 + int(to_string(abs(v[j])).size()));
        }
        bwidth = max(bwidth, int(to_string(b).size()));
    }
    static const vector<string> ineq{"<=", "=", ">="};
    for (int i = 0; i < m; i++) {
        const auto& [v, b, type] = smp.C[i];
        for (int j = 0; j < n; j++) {
            auto cell = format("{}{}", plus(j, v[j]), v[j]);
            s += format(" {:>{}}x{}", cell, width[j], j + 1);
        }
        s += format(" {} {:>{}}\n", ineq[int(type)], b, bwidth);
    }
    return s;
}

string format_tableau(const simplex& smp) {
    int n = smp.n, m = smp.m, s = smp.s, a = smp.a;
    const auto& tab = smp.tab;
    vector<string> labels(m + 1);
    vector<string> rows(m + 1);
    vector<size_t> width(tab.m, 0);
    size_t label_width = 0;
    string sep_row;

    auto repeat = [](int n, string s) {
        string str;
        while (n--)
            str += s;
        return str;
    };

    for (int i = 1; i <= m; i++) {
        int v = smp.row_to_var[i];
        if (1 <= v && v <= n)
            labels[i] = format("x{}", v);
        else if (n < v && v <= n + s)
            labels[i] = format("s{}", v - n);
        else if (n + s < v && v <= n + s + a)
            labels[i] = format("a{}", v - n - s);
        label_width = max(label_width, labels[i].size());
    }

    for (int i = 0; i <= m; i++) {
        for (int j = 0; j < tab.m; j++) {
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
        for (int j = n + s + 1; j < tab.m; j++) {
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
    for (int j = n + s + 1; j < tab.m; j++) {
        sep_row += repeat(width[j] + 2, "─");
    }

    string main = format("{}", fmt::join(begin(rows) + 1, end(rows), "\n"));
    return format("{}\n{}\n{}\n", rows[0], sep_row, main);
}

// return equivalent lp problem with only LP_LESS constraints
simplex standardize(const simplex& lp) {
    simplex smp;
    smp.set_objective(lp.z);
    for (int i = 0; i < lp.m; i++) {
        auto c = lp.C[i];
        if (c.type == LP_LESS) {
            smp.add_constraint(c);
        }
        if (c.type == LP_EQUAL) {
            c.type = LP_LESS, smp.add_constraint(c), c.type = LP_GREATER;
        }
        if (c.type == LP_GREATER) {
            c.type = LP_LESS;
            c.b = -c.b;
            for (auto& f : c.v)
                f = -f;
            smp.add_constraint(c);
        }
    }
    return smp;
}

// Given primal lp problem must be canonical (only LP_LESS constraints)
simplex make_dual(const simplex& primal) {
    int n = primal.n, m = primal.m;
    simplex dual;
    vector<frac> z(m);
    vector<lp_constraint> C(n);

    for (int i = 0; i < m; i++) {
        z[i] = -primal.C[i].b;
        assert(primal.C[i].type == LP_LESS);
    }
    for (int j = 0; j < n; j++) {
        C[j].v.resize(m);
        for (int i = 0; i < m; i++) {
            C[j].v[i] = -primal.C[i].v[j];
        }
        C[j].b = -primal.z[j];
        C[j].type = LP_LESS;
    }
    dual.set_objective(z);
    dual.add_constraints(C);
    return dual;
}

#endif // SIMPLEX_UTILS_HPP
