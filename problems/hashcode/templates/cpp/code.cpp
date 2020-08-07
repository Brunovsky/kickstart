#include <bits/stdc++.h>
#include <fmt/format.h>

using namespace std;

// *****

void solve() {}

// *****

int main(int argc, char** argv) {
    if (argc <= 1) {
        fmt::print("Missing input filename\n");
        return 1;
    }
    string filename = argv[1];
    ifstream input(filename);
    if (!input.is_open()) {
        fmt::print("Failed to open input '{}'\n", filename);
        return 1;
    }
    solve();
    return 0;
}
