
void compute(int N, int M, part_t b) {
    int S = b.size();
    assert(M >= S);
    int i = 0, prev_dices = 0, prev_m = M - S;
    double best = DBL_MAX;
    (void)N;

    while (i < S) {
        int j = i + 1;
        while (j < S && b[i] == b[j])
            j++;

        int dices = b[i];
        int m = dices - 1 == prev_dices ? prev_m + 1 : 1;

        dices == 1 ? b.erase(begin(b)), 0 : b[i]--;

        double rolls = 1.0 * M / m;
        best = min(best, dp.at(b) + rolls);

        dices == 1 ? b.insert(begin(b), 1), 0 : b[i]++;

        prev_dices = dices;
        prev_m = j - i;
        i = j;
    }

    show(b, best);
    dp.emplace(move(b), best);
}

void do_dp(int maxN, int M) {
    dp.clear();

    part_t base_case = {1};
    dp[base_case] = 1.0; // base case 1 roll

    for (int N = 2; N < maxN; N++) {
        part_t a(N + 1, 0);
        int k = 1;
        a[1] = N;

        while (k != 0) {
            int x = a[k - 1] + 1;
            int y = a[k] - 1;
            k--;
            while (x <= y) {
                a[k] = x;
                y -= x;
                k++;
            }
            a[k] = x + y;

            part_t b(begin(a), begin(a) + k + 1);
            int S = b.size();
            if (S <= M)
                compute(N, M, b);
        }
    }
}
