#include <bits/stdc++.h>

using namespace std;

#define long int64_t

// *****

inline void next_lexicographical_mask(uint& v) {
    uint c = v & -v, r = v + c;
    v = c ? (((r ^ v) >> 2) / c) | r : UINT_MAX;
}

#define combine(amask, bmask) ((amask) << M | (bmask))
#define geta(combined)        ((combined) >> M)
#define getb(combined)        ((combined) & ((1 << M) - 1))
#define without(mask, i)      ((mask) & ~(1 << i))
#define with(mask, i)         ((mask) | (1 << i))

/**
 * Iterate through the set bits of an unsigned integer mask
 * - mask: an unsigned integer mask (expression)
 * - n: name for variable holding the set bit index
 * - bit: name for variable holding the shifted bit (1 << n)
 * mask should not be 0.
 */
#define FOR_EACH_BIT(mask, n, bit)                                                      \
    for (uint z##n##bit = (mask), bit = z##n##bit & -z##n##bit, n = __builtin_ctz(bit); \
         z##n##bit; z##n##bit ^= bit,                                                   \
              z##n##bit && (bit = z##n##bit & -z##n##bit, (n = __builtin_ctz(bit))))

/**
 * Iterate through the integer masks with `size` set bits using the lowest `max_size`
 * bits only. In other words, iterate through all subsets with cardinality size of a
 * set with cardinality max_size.
 * - mask: name for variable holding the generated mask
 * - size: number of set bits in mask (expression).
 * - max_size: how many bit positions to iterate through (expression)
 * size and max_size should not be 0.
 */
#define FOR_EACH_MASK(mask, size, max_size)                                        \
    for (uint mask = (1 << (size)) - 1, m##mask = 1 << (max_size); mask < m##mask; \
         next_lexicographical_mask(mask))

/**
 * Iterate through the proper, non-empty subsets of an unsigned integer mask
 * - subset: name for variable holding subset mask.
 * - mask: an unsigned integer mask (expression)
 * subset != 0 and subset != mask.
 */
#define FOR_EACH_SUBSET(subset, mask)                                           \
    for (uint s##subset = (mask), subset = s##subset & (s##subset - 1); subset; \
         subset = s##subset & (subset - 1))

int N, M, E = 0;
int a[6], b[6], c[6][6], maskdelta[1 << 12], dp[1 << 12];
vector<uint> adj[1 << 12], rev[1 << 12];

int mask_gain(int chests_mask) {
    int ans = 0;
    for (int i = 0; i < N; i++)
        ans += (chests_mask & (1 << i)) ? a[i] : 0;
    return ans;
}
int mask_pay(int keys_mask) {
    int ans = 0;
    for (int i = 0; i < M; i++)
        ans += (keys_mask & (1 << i)) ? b[i] : 0;
    return ans;
}
int mask_chest_cost(int chest, int keys_mask) {
    int ans = 0;
    for (int i = 0; i < M; i++)
        ans += (keys_mask & (1 << i)) ? c[chest][i] : 0;
    return ans;
}
int mask_key_cost(int key, int chests_mask) {
    int ans = 0;
    for (int i = 0; i < N; i++) {
        ans += (chests_mask & (1 << i)) ? c[i][key] : 0;
    }
    return ans;
}

void add_edge(int a, int b) { adj[a].push_back(b), rev[b].push_back(a), E++; }

string bits(int mask, int Z) {
    string s;
    for (int i = 0; i < Z; i++)
        s += mask & (1 << i) ? '1' : '0';
    return s;
}

auto solve() {
    cin >> N >> M;

    for (int i = 0; i < N; i++) {
        cin >> a[i];
    }
    for (int i = 0; i < M; i++) {
        cin >> b[i];
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            cin >> c[i][j];
        }
    }
    if (accumulate(a, a + N, 0) > accumulate(b, b + M, 0)) {
        return -1; // Bob just buys all the keys
    }

    for (int blen = 1; blen <= M; blen++) {
        FOR_EACH_MASK (bmask, blen, M) {
            for (int alen = blen == M ? N : 0; alen <= N; alen++) {
                FOR_EACH_MASK (amask, alen, N) {
                    int gain = mask_gain(amask);
                    int pay = mask_pay(bmask);
                    int mask = combine(amask, bmask);
                    maskdelta[mask] = gain - pay;

                    if (maskdelta[mask] <= 0 && bmask > 0) {
                        FOR_EACH_BIT (bmask, n, _) {
                            FOR_EACH_SUBSET (subset, amask) {
                                int prev = combine(subset, without(bmask, n));
                                if (maskdelta[prev] <= 0) {
                                    add_edge(prev, mask);
                                }
                            }
                            int prev = combine(0, without(bmask, n));
                            if (maskdelta[prev] <= 0) {
                                add_edge(prev, mask);
                            }
                            prev = combine(amask, without(bmask, n));
                            if (maskdelta[prev] <= 0) {
                                add_edge(prev, mask);
                            }
                        }
                    }
                }
            }
        }
    }

    cerr << "edges: " << E << endl;

    memset(dp, 0x00, sizeof(dp));
    dp[combine((1 << N) - 1, (1 << M) - 1)] = 0;

    for (int blen = M - 1; blen >= 0; blen--) {
        FOR_EACH_MASK (subb, blen, M) {
            for (int alen = 0; alen <= N; alen++) {
                FOR_EACH_MASK (suba, alen, N) {
                    int sub = combine(suba, subb);
                    if (maskdelta[sub] <= 0) {
                        vector<int> best(M, 0), best_cost(M, INT_MAX);
                        for (int mask : adj[sub]) {
                            uint amask = geta(mask), bmask = getb(mask);
                            int new_chests = amask & ~suba;
                            int new_key = __builtin_ctz(bmask & ~subb);

                            assert(__builtin_popcount(bmask) == blen + 1);
                            assert((suba & amask) == suba);
                            assert(0 <= new_key && new_key < M);

                            int cost = mask_key_cost(new_key, new_chests);
                            best_cost[new_key] = min(best_cost[new_key], cost);
                            best[new_key] = max(best[new_key], dp[mask] + cost);
                        }
                        dp[sub] = 0;
                        for (int k = 0; k < M; k++) {
                            if ((subb & (1 << k)) == 0) {
                                printf("best[%d] = %d\n", k, best_cost[k]);
                                dp[sub] += best_cost[k];
                            }
                        }
                        printf("dp[%s][%s] = %d\n", bits(suba, N).data(),
                               bits(subb, M).data(), dp[sub]);
                    }
                }
            }
        }
    }

    return dp[0];
}

// *****

int main() {
    cout << solve() << endl;
    return 0;
}
