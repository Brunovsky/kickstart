#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        int N, L;
        cin >> N >> L;
        unordered_map<string, int> idmap;

        vector<string> lines(N);
        for (int i = 0; i < N; i++) {
            cin >> lines[i];
        }
        vector<int> ids(L);
        for (int i = 0; i < L; i++) {
            string col;
            for (int j = 0; j < N; j++) {
                col += lines[j][i];
            }
            ids[i] = idmap.emplace(col, idmap.size() + 1).first->second;
        }
        cout << idmap.size() << endl;
        for (int i = 0; i < L; i++) {
            cout << ids[i] << " \n"[i + 1 == L];
        }
    }
    return 0;
}
