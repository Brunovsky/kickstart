#include <bits/stdc++.h>

using namespace std;

// *****

unordered_map<string, int> id_map;
vector<set<int>> deps;
int next_id;

static regex rgx("^([a-z]+)=[a-z]+\\(((?:[a-z]+(?:,[a-z]+)*)?)\\)$"s,
                 regex_constants::ECMAScript | regex_constants::optimize);

int accept_one(const string &var) {
    if (id_map.count(var))
        return id_map[var];
    id_map.emplace(var, next_id);
    return next_id++;
}

vector<int> accept_many(const string &vars) {
    int i = 0, S = vars.size();
    vector<int> ids;
    while (i < S) {
        int j = i + 1;
        while (j < S && vars[j] != ',')
            ++j;
        ids.push_back(accept_one(vars.substr(i, j - i)));
        i = j + 1;
    }
    return ids;
}

auto solve() {
    int N;
    cin >> N >> ws;
    id_map.clear();
    deps.assign(N, {});
    next_id = 0;

    for (int i = 0; i < N; ++i) {
        string line;
        getline(cin, line);
        smatch match;
        regex_match(line, match, rgx);

        string lvalue_str = match[1];
        string rvalues_str = match[2];

        int lvalue = accept_one(lvalue_str);
        vector<int> rvalues = accept_many(rvalues_str);

        // more variables than equations means busted
        if (next_id > N) {
            while (++i < N)
                getline(cin, line);
            return "BAD";
        }

        for (int arg : rvalues) {
            deps[lvalue].insert(arg);
        }
    }

    vector<bool> removed(N, false);

    for (int i = 0; i < N; ++i) {
        int found = -1;
        for (int k = 0; k < N; ++k) {
            if (!removed[k] && deps[k].empty()) {
                found = k;
                break;
            }
        }
        // the remaining nodes form a cyclic graph, busted
        if (found == -1)
            return "BAD";

        removed[found] = true;
        for (int k = 0; k < N; ++k) {
            deps[k].erase(found);
        }
    }

    return "GOOD";
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
