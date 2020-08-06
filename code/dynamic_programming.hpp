#include <bits/stdc++.h>

using namespace std;

// *****

string longest_common_subsequence(string a, string b) {
    int A = a.size(), B = b.size();
    vector<vector<int>> dp(A + 1, vector<int>(B + 1, 0));

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if (a[i] == b[j])
                dp[i + 1][j + 1] = 1 + dp[i][j];
            else
                dp[i + 1][j + 1] = max(dp[i + 1][j], dp[i][j + 1]);
        }
    }

    string subsequence;
    int i = A - 1, j = B - 1;
    while (i >= 0 && j >= 0 && dp[i + 1][j + 1]) {
        if (a[i] == b[j])
            subsequence.push_back(a[i]), i--, j--;
        else if (dp[i + 1][j + 1] == dp[i][j + 1])
            i--;
        else
            j--;
    }
    reverse(begin(subsequence), end(subsequence));
    return subsequence;
}

string longest_common_substring(string a, string b) {
    int A = a.size(), B = b.size();
    vector<int> prev(B + 1, 0);
    vector<int> next(B + 1, 0);

    int L = 0;
    string substring;

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if (a[i] == b[j]) {
                next[j + 1] = 1 + prev[j];
                if (L < next[j + 1]) {
                    L = next[j + 1];
                    substring = a.substr(i - L, L);
                }
            }
        }
        swap(prev, next);
    }

    return substring;
}

/**
 * distance from a to b
 * del: cost of deletion of one character (from a)
 * ins: cost of insertion of one character (from b into a)
 * sub: cost of substitution of one character
 */
int levenshtein_distance(string a, string b, int del, int ins, int sub) {
    int A = a.size(), B = b.size();
    vector<int> prev(B + 1, 0);
    vector<int> next(B + 1, 0);

    for (int j = 0; j < B; j++)
        prev[j + 1] = (j + 1) * ins;

    for (int i = 0; i < A; i++) {
        next[0] = (i + 1) * del;
        for (int j = 0; j < B; j++) {
            int eqs = a[i] == b[j] ? 0 : sub;
            int del_dist = del + next[j];
            int ins_dist = ins + prev[j + 1];
            int sub_dist = eqs + prev[j];
            next[j + 1] = min(min(ins_dist, del_dist), sub_dist);
        }
        swap(prev, next);
    }

    return prev[B];
}

/**
 * distance from a to b
 * del: cost of deletion of one character (from a)
 * ins: cost of insertion of one character (from b into a)
 * sub: cost of substitution of one character
 * tra: cost of transposition of two characters (in a)
 */
int simple_damerau_distance(string a, string b, int del, int ins, int sub, int tra) {
    int A = a.size(), B = b.size();
    vector<int> tran(B + 1, 0);
    vector<int> prev(B + 1, 0);
    vector<int> next(B + 1, 0);

    for (int j = 0; j < B; j++)
        prev[j + 1] = (j + 1) * ins;

    for (int i = 0; i < A; i++) {
        next[0] = (i + 1) * del;
        for (int j = 0; j < B; j++) {
            int eqs = a[i] == b[j] ? 0 : sub;
            int del_dist = del + next[j];
            int ins_dist = ins + prev[j + 1];
            int sub_dist = eqs + prev[j];
            next[j + 1] = min(min(ins_dist, del_dist), sub_dist);
            if (i && j && a[i] == b[j - 1] && a[i - 1] == b[j]) {
                int tra_dist = tra + tran[j - 1];
                next[j + 1] = min(next[j + 1], tra_dist);
            }
        }

        auto prev_next = next;
        auto prev_prev = prev;
        auto prev_tran = tran;
        swap(tran, next);
        swap(prev, tran);
    }

    return prev[B];
}

/**
 * distance from a to b
 * del: cost of deletion of one character (from a)
 * ins: cost of insertion of one character (from b into a)
 * sub: cost of substitution of one character
 * tra: cost of transposition of two characters (in a)
 */
int damerau_distance(string a, string b, int del, int ins, int sub, int tra) {
    int A = a.size(), B = b.size();
    vector<vector<int>> dp(A + 2, vector<int>(B + 2, 0));
    array<int, 256> finger = {};

    const int inf = (A + B) * max(max(del, ins), max(sub, tra));
    fill(begin(dp[0]), end(dp[0]), inf);
    dp[1][0] = inf;
    for (int j = 0; j < B; j++)
        dp[1][j + 2] = (j + 1) * ins;

    for (int i = 0; i < A; i++) {
        dp[i + 2][0] = inf;
        dp[i + 2][1] = (i + 1) * del;
        int jp = 0; // the largest jj<j for which a[i] == b[jj]
        for (int j = 0; j < B; j++) {
            int eqs = a[i] == b[j] ? 0 : sub;
            int ip = finger[int(b[j])]; // the largest ii<i for which a[ii] == b[j]
            int swp = (i - ip + j - jp + 1) * tra;
            int del_dist = del + dp[i + 2][j + 1];
            int ins_dist = ins + dp[i + 1][j + 2];
            int sub_dist = eqs + dp[i + 1][j + 1];
            int tra_dist = swp + dp[ip][jp];
            dp[i + 2][j + 2] = min(min(ins_dist, del_dist), min(sub_dist, tra_dist));
            if (a[i] == b[j])
                jp = j + 1;
        }
        finger[int(a[i])] = i + 1;
    }

    return dp[A + 1][B + 1];
}
