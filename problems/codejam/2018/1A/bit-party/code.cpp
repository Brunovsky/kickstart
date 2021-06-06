#include <bits/stdc++.h>

using namespace std;

// *****

#define PUSH_MIN_HEAP(heap, val) \
    heap.push_back(val);         \
    push_heap(begin(heap), end(heap), greater<long>{})
#define POP_MIN_HEAP(heap)                             \
    pop_heap(begin(heap), end(heap), greater<long>{}); \
    heap.pop_back()

#define MAXC 1000

long R, B, C;
long M[MAXC];
long S[MAXC];
long P[MAXC];

auto solve() {
    cin >> R >> B >> C;
    long lo = 1, hi = 0;
    for (int i = 0; i < C; i++) {
        cin >> M[i] >> S[i] >> P[i];
        hi = max(hi, S[i] * M[i] + P[i]);
    }
    while (lo <= hi) {
        long T = (lo + hi) / 2;
        vector<long> heap;
        heap.reserve(R + 1);
        for (int i = 0; i < C; i++) {
            if (T > P[i]) {
                long bits = min(M[i], (T - P[i]) / S[i]);
                PUSH_MIN_HEAP(heap, bits);
                if (heap.size() > size_t(R)) {
                    POP_MIN_HEAP(heap);
                }
            }
        }
        long bits = 0;
        for (uint i = 0; i < heap.size(); i++) {
            bits += heap[i];
            if (bits >= B) {
                break; // be careful with overflow
            }
        }
        if (bits >= B) {
            hi = T - 1;
        } else {
            lo = T + 1;
        }
    }
    return lo;
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
