#include <bits/stdc++.h>

using namespace std;

// *****

struct point_t {
    double X, Y, W;
};

bool cmp_p1(point_t lhs, point_t rhs) {
    double Sl = lhs.X - lhs.Y;
    double Sr = rhs.X - rhs.Y;
    return make_tuple(Sl, lhs.X, lhs.Y) < make_tuple(Sr, rhs.X, rhs.Y);
}

bool cmp_n1(point_t lhs, point_t rhs) {
    double Sl = lhs.X + lhs.Y;
    double Sr = rhs.X + rhs.Y;
    return make_tuple(Sl, lhs.X, lhs.Y) < make_tuple(Sr, rhs.X, rhs.Y);
}

auto solve() {
    int N;
    cin >> N >> ws;

    if (N == 1)
        return 0.0;

    vector<point_t> A(N);
    vector<point_t> B(N);
    double totalW = 0.0;
    double min_X = 1000.0, min_Y = 1000.0;

    for (int i = 0; i < N; ++i) {
        point_t point;
        cin >> point.X >> point.Y >> point.W >> ws;
        A[i] = B[i] = point;
        totalW += point.W;
        min_X = min(min_X, point.X);
        min_Y = min(min_Y, point.Y);
    }

    sort(A.begin(), A.end(), cmp_n1);
    sort(B.begin(), B.end(), cmp_p1);

    double sX = min_X - 1.0, sY = min_Y - 1.0;
    double c1 = sX + sY, c2 = sX - sY;

    double left = 0.0, right = totalW;
    for (int ai = 0; ai < N; ++ai) {
        point_t point = A[ai];
        c1 = point.X + point.Y;
        right -= point.W;
        left += point.W;
        if (left - right >= 0) {
            break;
        }
    }

    left = 0.0, right = totalW;
    for (int bi = 0; bi < N; ++bi) {
        point_t point = B[bi];
        c2 = point.X - point.Y;
        right -= point.W;
        left += point.W;
        if (left - right >= 0) {
            break;
        }
    }

    // c1 == X + Y      and  c2 == X - Y
    // X = (c1 + c2)/2  and  Y = (c1 - c2)/2
    double X = (c1 + c2) / 2.0;
    double Y = (c1 - c2) / 2.0;

    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        point_t point = A[i];
        sum += max(abs(X - point.X), abs(Y - point.Y)) * point.W;
    }
    return sum;
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(7);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
