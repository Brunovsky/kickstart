#include "../linear/transportation.hpp"

#include "test_utils.hpp"

// clang-format off

void unit_test_transportation() {
    mat<int> solution, tp;
    long ans;

    ans = transportation::solve({{{100, 50}, {80, 30, 40}}}, {{
        {5, 3, 2},
        {2, 2, 1},
    }}, &tp);
    solution = {{{30, 30, 40}, {50, 0, 0}}};
    assert(tp == solution && ans == 420);

    ans = transportation::solve({{{100, 60}, {100, 20, 40}}}, {{
        {2, 2, 5},
        {3, 0, 3},
    }}, &tp);
    solution = {{{100, 0, 0}, {0, 20, 40}}};
    assert(tp == solution && ans == 320);

    ans = transportation::solve({{{50, 50, 50}, {50, 25, 45, 30}}}, {{
        {0, 2, 2, 7},
        {0, 5, 1, 2},
        {0, 1, 4, 3},
    }}, &tp);
    solution = {{{50, 0, 0, 0}, {0, 0, 45, 5}, {0, 25, 0, 25}}};
    assert(tp == solution && ans == 155);

    ans = transportation::solve({{{123, 143, 84, 150}, {200, 100, 200}}}, {{
        {17, 24, 19},
        {15, 21, 25},
        {19, 22, 18},
        {20, 27, 16},
    }}, &tp);
    solution = {{{123, 0, 0}, {77, 66, 0}, {0, 34, 50}, {0, 0, 150}}};
    assert(tp == solution && ans == 8680);

    ans = transportation::solve({{{134, 203, 176, 187}, {175, 175, 175, 175}}}, {{
        {56, 86, 80, 61},
        {59, 76, 78, 65},
        {62, 70, 57, 67},
        {60, 68, 75, 71},
    }}, &tp);
    solution = {{{0, 0, 0, 134}, {163, 0, 0, 40}, {0, 0, 175, 1}, {12, 175, 0, 0}}};
    assert(tp == solution && ans == 43053);

    ans = transportation::solve({{{54, 67, 29}, {21, 32, 51, 46}}}, {{
        {27, 33, 34, 41},
        {31, 29, 37, 30},
        {40, 32, 28, 35},
    }}, &tp);
    solution = {{{21, 11, 22, 0}, {0, 21, 0, 46}, {0, 0, 29, 0}}};
    assert(tp == solution && ans == 4479);

    ans = transportation::solve({{{17, 23, 10}, {15, 30, 25}}}, {{
        {7, 4, 3},
        {9, 2, 1},
        {8, 6, 3},
    }}, &tp);
    solution = {{{0, 2, 15}, {0, 23, 0}, {0, 0, 10}}};
    assert(tp == solution && ans == 129);

    ans = transportation::solve({{{74, 92, 81, 77, 76}, {80, 60, 50, 90, 30, 90}}}, {{
        {65, 75, 10, 6, 16, 62},
        {94, 30, 13, 4, 0, 71},
        {98, 75, 96, 81, 48, 13},
        {12, 13, 4, 36, 2, 70},
        {25, 74, 26, 62, 36, 14},
    }}, &tp);
    solution = {{{0, 0, 46, 28, 0, 0},
                {0, 0, 0, 62, 30, 0},
                {0, 0, 0, 0, 0, 81},
                {13, 60, 4, 0, 0, 0},
                {67, 0, 0, 0, 0, 9}}};
    assert(tp == solution && ans == 4682);
}

int main() {
    RUN_SHORT(unit_test_transportation());
    return 0;
}
