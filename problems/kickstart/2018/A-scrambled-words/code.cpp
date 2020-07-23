#include <bits/stdc++.h>

using namespace std;

// *****

using letters = array<int, 26>;

struct hasher {
    inline size_t operator()(const letters &vec) const noexcept {
        size_t seed = 73;
        for (auto i : vec) {
            seed ^= size_t(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

int L, N;
vector<string> dictionary_words;
string text;

void read() {
    cin >> L >> ws;
    dictionary_words.resize(L);
    for (int i = 0; i < L; i++) {
        cin >> dictionary_words[i] >> ws;
    }
    char c1, c2;
    long A, B, C, D;
    cin >> c1 >> ws >> c2 >> ws >> N >> A >> B >> C >> D >> ws;
    text.resize(N + 1);
    text[0] = c1;
    text[1] = c2;
    long x1 = c1 + 0, x2 = c2 + 0;
    for (int i = 2; i < N; i++) {
        long x = (A * x2 + B * x1 + C) % D;
        text[i] = 'a' + (x % 26);
        x1 = x2;
        x2 = x;
    }
    text[N] = '.';
}

map<int, unordered_multiset<letters, hasher>[26][26]> all_dictionary;
map<int, int> lengths_count;

auto count_letters(string substring) {
    letters lett = {};
    int W = substring.size();
    for (int i = 0; i < W; i++) {
        lett[substring[i] - 'a']++;
    }
    return lett;
}

auto solve() {
    read();
    lengths_count.clear();
    all_dictionary.clear();
    for (const string &word : dictionary_words) {
        int W = word.size();
        if (W > N) {
            continue;
        }
        int F = word.front() - 'a';
        int B = word.back() - 'a';
        all_dictionary[W][F][B].insert(count_letters(word.substr(1, W - 2)));
        lengths_count[W]++;
    }
    int answer = 0;
    for (auto it = all_dictionary.begin(); it != all_dictionary.end(); it++) {
        int W = it->first;
        auto &dictionary = it->second;
        int answer_W = 0;
        int total_W = lengths_count[W];
        int f = 0;
        int b = W - 1;
        letters lett = count_letters(text.substr(1, W - 2));
        do {
            int F = text[f] - 'a';
            int B = text[b] - 'a'; // b < N
            answer_W += dictionary[F][B].erase(lett);
            lett[text[++f] - 'a']--;
            lett[text[b++] - 'a']++;
        } while (b < N && answer_W < total_W);
        answer += answer_W;
    }
    return answer;
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
