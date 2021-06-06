#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int SIDE_MAX = 500'000'000;

struct star_t {
    int X, Y, Z, R;
};

struct cube_t {
    int X_min = SIDE_MAX, X_max = -SIDE_MAX;
    int Y_min = SIDE_MAX, Y_max = -SIDE_MAX;
    int Z_min = SIDE_MAX, Z_max = -SIDE_MAX;
};

int N;
vector<star_t> stars;
cube_t global;

inline void update(star_t star, cube_t &cube) {
    cube.X_min = min(cube.X_min, star.X - star.R);
    cube.X_max = max(cube.X_max, star.X + star.R);
    cube.Y_min = min(cube.Y_min, star.Y - star.R);
    cube.Y_max = max(cube.Y_max, star.Y + star.R);
    cube.Z_min = min(cube.Z_min, star.Z - star.R);
    cube.Z_max = max(cube.Z_max, star.Z + star.R);
}

inline int side(cube_t cube) {
    int x = cube.X_max - cube.X_min;
    int y = cube.Y_max - cube.Y_min;
    int z = cube.Z_max - cube.Z_min;
    return max(x, max(y, z));
}

inline bool within(star_t star, cube_t cube) {
    return cube.X_min <= star.X - star.R && star.X + star.R <= cube.X_max &&
           cube.Y_min <= star.Y - star.R && star.Y + star.R <= cube.Y_max &&
           cube.Z_min <= star.Z - star.R && star.Z + star.R <= cube.Z_max;
}

bool good(int L, int x, int y, int z) {
    cube_t one, two;
    one.X_min = x == -1 ? global.X_min : global.X_max - L;
    one.X_max = x == -1 ? global.X_min + L : global.X_max;
    one.Y_min = y == -1 ? global.Y_min : global.Y_max - L;
    one.Y_max = y == -1 ? global.Y_min + L : global.Y_max;
    one.Z_min = z == -1 ? global.Z_min : global.Z_max - L;
    one.Z_max = z == -1 ? global.Z_min + L : global.Z_max;

    for (star_t star : stars) {
        if (!within(star, one)) {
            update(star, two);
        }
    }

    return side(two) <= L;
}

int binary_search(int x, int y, int z) {
    int L = 1, R = side(global);

    while (L <= R) {
        int M = (L + R) / 2;
        if (good(M, x, y, z)) {
            R = M - 1;
        } else {
            L = M + 1;
        }
    }

    return L;
}

const vector<array<int, 3>> vertices = {
    {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
    {1, -1, -1},  {1, -1, 1},  {1, 1, -1},  {1, 1, 1},
};

auto solve() {
    global = cube_t();

    cin >> N >> ws;
    stars.resize(N);

    for (int i = 0; i < N; ++i) {
        cin >> stars[i].X >> stars[i].Y >> stars[i].Z >> stars[i].R >> ws;
        update(stars[i], global);
    }

    int best = INT_MAX;
    for (auto v : vertices) {
        best = min(best, binary_search(v[0], v[1], v[2]));
    }
    return best;
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
