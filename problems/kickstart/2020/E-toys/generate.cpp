#include <bits/stdc++.h>

using namespace std;

// *****

default_random_engine mt(random_device{}());
using intd = uniform_int_distribution<int>;
using longd = uniform_int_distribution<long>;

#define T 1000

intd distn(1, 8);
longd diste(1, 10);
longd distr(1, 100);

int main() {
    cout << T << endl;
    for (int t = 0; t < T; t++) {
        int n = distn(mt);
        cout << n << endl;
        for (int i = 0; i < n; i++) {
            cout << diste(mt) << ' ' << distr(mt) << endl;
        }
    }
    return 0;
}
