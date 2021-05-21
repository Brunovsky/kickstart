#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100'000
#define MAXG 100'001

int N, G, M;
int guestid[MAXG];  // guest index -> guest id
int countmem[MAXG]; // guest id -> number of consulates
int idmap[MAXN][2]; // position -> guest id
int memid[MAXN][2]; // id memorized by consulate i, each direction
int ndays[MAXN][2]; // days since the id memorized visited consulate i

inline int left(int x) {
    return x == 0 ? N - 1 : x - 1;
}

inline int right(int x) {
    return x == N - 1 ? 0 : x + 1;
}

inline int move(int x, int c) {
    return c ? right(x) : left(x);
}

inline int consul(int x) {
    x = x % N;
    return x < 0 ? x + N : x;
}

// c=0 anticlockwise, c=1 clockwise
void walk(int c) {
    int max_days = min(N, M);
    int x = consul(c ? -M : M);
    int days = 0;
    while (days < N && !idmap[x][c]) {
        x = move(x, c);
        days++;
    }
    if (days == N) {
        return;
    }
    memid[0][c] = days <= max_days ? idmap[x][c] : 0;
    ndays[0][c] = days--;
    for (int i = move(0, c); i != 0; i = move(i, c)) {
        if (days < 0) {
            do {
                x = move(x, c);
                days++;
            } while (days < N && !idmap[x][c]);
        }
        memid[i][c] = days <= max_days ? idmap[x][c] : 0;
        ndays[i][c] = days--;
    }
}

auto solve() {
    memset(guestid, 0, sizeof(guestid));
    memset(countmem, 0, sizeof(countmem));
    memset(idmap, 0, sizeof(idmap));
    memset(memid, 0, sizeof(memid));
    memset(ndays, 0, sizeof(ndays));

    int gids = 1;

    cin >> N >> G >> M >> ws;
    for (int g = 1; g <= G; g++) {
        int x;
        char direction;
        cin >> x >> ws >> direction >> ws;
        bool c = direction == 'C';
        x--;
        if (!idmap[x][c]) {
            idmap[x][c] = gids++;
        }
        guestid[g] = idmap[x][c];
    }

    walk(0);
    walk(1);

    for (int i = 0; i < N; i++) {
        int ida = memid[i][0], idc = memid[i][1];
        if (ida == 0 && idc == 0) {
            continue;
        }
        if (ida == 0) {
            countmem[idc]++;
            continue;
        }
        if (idc == 0) {
            countmem[ida]++;
            continue;
        }
        int ta = ndays[i][0], tc = ndays[i][1];
        if (ta == tc) {
            countmem[ida]++;
            countmem[idc]++;
        } else if (ta < tc) {
            countmem[ida]++;
        } else {
            countmem[idc]++;
        }
    }
    for (int g = 1; g <= G; g++) {
        int memorized = countmem[guestid[g]];
        cout << ' ' << memorized;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
        cout << '\n';
    }
    return 0;
}
