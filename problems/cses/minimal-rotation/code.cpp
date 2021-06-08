#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

template <typename Vec>
int min_cyclic_string(Vec s) {
    s.reserve(2 * s.size());
    s.insert(end(s), begin(s), end(s));
    int N = s.size(), i = 0, ans = 0;
    while (i < N / 2) {
        ans = i;
        int j = i + 1, k = i;
        while (j < N && s[k] <= s[j])
            s[k] < s[j] ? k = i : k++, j++;
        while (i <= k)
            i += j - k;
    }
    return ans;
}

int main() {
    ios::sync_with_stdio(false);
    string s;
    cin >> s;
    int i = min_cyclic_string(s);
    s = s.substr(i) + s.substr(0, i);
    cout << s << endl;
    return 0;
}
