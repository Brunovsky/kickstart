#include <bits/stdc++.h>

using namespace std;

// *****

struct soldier_t {
    int A, D;

    friend bool operator==(soldier_t lhs, soldier_t rhs) {
        return lhs.A == rhs.A && lhs.D == rhs.D;
    }
};

int N;
vector<soldier_t> soldiers;
vector<int> As;
vector<int> Ds;
bool W[4000][4000];

bool A_cmp(int i, int j) {
    int Ai = soldiers[i].A, Aj = soldiers[j].A;
    return Ai < Aj || (Ai == Aj && i < j);
}

bool D_cmp(int i, int j) {
    int Di = soldiers[i].D, Dj = soldiers[j].D;
    return Di < Dj || (Di == Dj && i < j);
}

void prepare() {
    As.resize(N);
    Ds.resize(N);
    iota(begin(As), end(As), 0);
    iota(begin(Ds), end(Ds), 0);
    sort(begin(As), end(As), A_cmp);
    sort(begin(Ds), end(Ds), D_cmp);

    int i, k;
    i = k = 0;
    while (i < N) {
        int n = soldiers[As[i]].A;
        do {
            soldiers[As[i++]].A = k;
        } while (i < N && soldiers[As[i]].A == n);
        ++k;
    }
    i = k = 0;
    while (i < N) {
        int n = soldiers[Ds[i]].D;
        do {
            soldiers[Ds[i++]].D = k;
        } while (i < N && soldiers[Ds[i]].D == n);
        ++k;
    }
}

bool visit(int a, int d) {
    soldier_t A = soldiers[As[a]];
    soldier_t D = soldiers[Ds[d]];

    if (A.D > D.D || A.A < D.A || W[A.A][D.D]) {
        return false;
    }

    // the current bound can be achieved from first play
    if (A == D) {
        return true;
    }

    for (int i = 0; i < a; ++i) {
        soldier_t u = soldiers[As[i]];
        if (u.A >= A.A) {
            break;
        }
        if (u.D <= D.D) {
            W[u.A][D.D] = true;
        }
    }

    for (int i = 0; i < d; ++i) {
        soldier_t u = soldiers[Ds[i]];
        if (u.D >= D.D) {
            break;
        }
        if (u.A <= A.A) {
            W[A.A][u.D] = true;
        }
    }

    return false;
}

auto solve() {
    cin >> N >> ws;
    soldiers.resize(N);
    for (int i = 0; i < N; ++i) {
        cin >> soldiers[i].A >> soldiers[i].D >> ws;
    }

    prepare();
    memset(W, 0, sizeof(W));

    // traverse graph in reverse topological order
    for (int i = N - 1; i >= 0; --i) {
        for (int j = N - 1; j >= 0; --j) {
            if (visit(i, j)) {
                return "YES";
            }
        }
    }

    return "NO";
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
