#include "../transportation.hpp"

// *****

void test() {
    transportation tp;
    vector<vector<int>> solution;
    tp.supply = {100, 50};
    tp.demand = {80, 30, 40};
    tp.cost = {
        {5, 3, 2},
        {2, 2, 1},
    };
    tp.compute();
    cout << to_string(tp.tp) << endl;
    solution = {{30, 30, 40}, {50, 0, 0}};
    assert(tp.tp == solution);

    tp.supply = {100, 60};
    tp.demand = {100, 20, 40};
    tp.cost = {
        {2, 2, 5},
        {3, 0, 3},
    };
    tp.compute();
    cout << to_string(tp.tp) << endl;
    solution = {{100, 0, 0}, {0, 20, 40}};
    assert(tp.tp == solution);

    tp.supply = {50, 50, 50};
    tp.demand = {50, 25, 45, 30};
    tp.cost = {
        {0, 2, 2, 7},
        {0, 5, 1, 2},
        {0, 1, 4, 3},
    };
    tp.compute();
    cout << to_string(tp.tp) << endl;
    solution = {{50, 0, 0, 0}, {0, 0, 45, 5}, {0, 25, 0, 25}};
    assert(tp.tp == solution);

    tp.supply = {123, 143, 84, 150};
    tp.demand = {200, 100, 200};
    tp.cost = {
        {17, 24, 19},
        {15, 21, 25},
        {19, 22, 18},
        {20, 27, 16},
    };
    tp.compute();
    cout << to_string(tp.tp) << endl;
    solution = {{123, 0, 0}, {77, 66, 0}, {0, 34, 50}, {0, 0, 150}};
    assert(tp.tp == solution);

    tp.supply = {134, 203, 176, 187};
    tp.demand = {175, 175, 175, 175};
    tp.cost = {
        {56, 86, 80, 61},
        {59, 76, 78, 65},
        {62, 70, 57, 67},
        {60, 68, 75, 71},
    };
    tp.compute();
    cout << to_string(tp.tp) << endl;
    solution = {{0, 0, 0, 134}, {163, 0, 0, 40}, {0, 0, 175, 1}, {12, 175, 0, 0}};
    assert(tp.tp == solution);

    tp.supply = {54, 67, 29};
    tp.demand = {21, 32, 51, 46};
    tp.cost = {
        {27, 33, 34, 41},
        {31, 29, 37, 30},
        {40, 32, 28, 35},
    };
    tp.compute();
    cout << to_string(tp.tp) << endl;
    solution = {{21, 11, 22, 0}, {0, 21, 0, 46}, {0, 0, 29, 0}};
    assert(tp.tp == solution);

    tp.supply = {17, 23, 10}; // 50
    tp.demand = {15, 30, 25}; // 70
    tp.cost = {
        {7, 4, 3},
        {9, 2, 1},
        {8, 6, 3},
    };
    tp.compute();
    cout << to_string(tp.tp) << endl;
    solution = {{0, 2, 15}, {0, 23, 0}, {0, 0, 10}};
    assert(tp.tp == solution);

    tp.supply = {74, 92, 81, 77, 76};
    tp.demand = {80, 60, 50, 90, 30, 90};
    tp.cost = {
        {65, 75, 10, 6, 16, 62},  //
        {94, 30, 13, 4, 0, 71},   //
        {98, 75, 96, 81, 48, 13}, //
        {12, 13, 4, 36, 2, 70},   //
        {25, 74, 26, 62, 36, 14}, //
    };
    tp.compute();
    cout << to_string(tp.tp) << endl;
    solution = {{0, 0, 46, 28, 0, 0},
                {0, 0, 0, 62, 30, 0},
                {0, 0, 0, 0, 0, 81},
                {13, 60, 4, 0, 0, 0},
                {67, 0, 0, 0, 0, 9}};
    assert(tp.tp == solution);
}

int main() {
    test();
    return 0;
}
