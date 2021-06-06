#include <bits/stdc++.h>

using namespace std;

// *****

struct Cell {
    int x, y;
};

bool operator==(Cell lhs, Cell rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

#define MAXX 30
#define MAXY 30

int A;
Cell RECT;
constexpr Cell DONE = {0, 0};
bool is_painted[MAXX][MAXY];
const unordered_map<int, Cell> rects = {
    {20, {4, 5}}, {50, {5, 10}}, {100, {10, 10}}, {200, {10, 20}}, {300, {15, 20}},
};

#define FOR_EACH_AROUND(cell, dx, dy) \
    for (int dx : {-1, 0, 1})         \
        for (int dy : {-1, 0, 1})

bool bounded(Cell cell) {
    return cell.x > 1 && cell.x < RECT.x && cell.y > 1 && cell.y < RECT.y;
}

int unpainted(Cell cell) {
    if (!bounded(cell)) {
        return 0;
    }
    int cnt = 0;
    FOR_EACH_AROUND(cell, dx, dy) {
        cnt += !is_painted[cell.x + dx][cell.y + dy];
    }
    return cnt;
}

struct least_painted {
    bool operator()(Cell a, Cell b) const noexcept {
        return make_tuple(unpainted(a), a.x, a.y) > make_tuple(unpainted(b), b.x, b.y);
    }
};

set<Cell, least_painted> cells;

Cell WRITE_READ(Cell cell) {
    cout << cell.x << ' ' << cell.y << endl;
    cin >> cell.x >> cell.y;
    assert(cell.x != -1 && cell.y != -1);
    return cell;
}

void solve() {
    cells.clear();
    memset(is_painted, 0, sizeof(is_painted));
    cin >> A;
    RECT = rects.at(A);
    for (int x = 2; x < RECT.x; x++) {
        for (int y = 2; y < RECT.y; y++) {
            cells.insert({x, y});
        }
    }
    int rounds = 0;
    while (true) {
        Cell cell = *cells.begin();
        Cell painted = WRITE_READ(cell);
        rounds++;

        if (painted == DONE) {
            break;
        }
        if (is_painted[painted.x][painted.y]) {
            continue;
        }

        FOR_EACH_AROUND(painted, dx, dy) {
            Cell around = {painted.x + dx, painted.y + dy};
            if (bounded(around)) {
                cells.erase(around);
            }
        }
        is_painted[painted.x][painted.y] = true;
        FOR_EACH_AROUND(painted, dx, dy) {
            Cell around = {painted.x + dx, painted.y + dy};
            if (bounded(around) && unpainted(around) != 0) {
                cells.insert(around);
            }
        }
    }
    fprintf(stderr, "A=%d | rounds: %d\n", A, rounds);
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
