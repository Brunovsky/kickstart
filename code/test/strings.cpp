#include "../strings/strings.hpp"

#include "../formatting.hpp"
#include "test_utils.hpp"

void unit_test_manachers() {
    string ss[] = {
        "abcdfdcecba"s,
        "abababc"s,
        "abaabbbaaaacca"s,
    };
    for (string& s : ss) {
        cout << build_manachers(s) << endl;
    }
}

void unit_test_prefix_function() {
    string ss[] = {
        "abcabcd"s, // 0,0,0,1,2,3,0
        "aabaaab"s, // 0,1,0,1,2,2,3
    };
    for (string& s : ss) {
        cout << build_prefix_function(s) << endl;
    }
}

void unit_test_z_function() {
    string ss[] = {
        "aaaaa"s,   // 0,4,3,2,1
        "aaabaab"s, // 0,2,1,0,2,1,0
        "abacaba"s, // 0,0,1,0,3,0,1
    };
    for (string& s : ss) {
        cout << build_z_function(s) << endl;
    }
}

void unit_test_lyndon() {
    string ss[] = {
        "efdcba"s,         // ef d c b a | 0,2,3,4,5
        "abaabaaabaaaab"s, // ab aab aaab aaaab | 0,2,5,9
        "abacaba"s,        // abac ab a | 0,4,6
    };
    for (string& s : ss) {
        cout << build_lyndon_factorization(s) << endl;
    }
}

int main() {
    RUN_SHORT(unit_test_manachers());
    RUN_SHORT(unit_test_prefix_function());
    RUN_SHORT(unit_test_z_function());
    RUN_SHORT(unit_test_lyndon());
    return 0;
}
