#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  inline bool neighbours(const string &s1, const string &s2) const {
    int count = 0;
    for (size_t i = 0; i < s1.size(); ++i) {
      count += s1[i] != s2[i];
      if (count == 2)
        return false;
    }
    return count == 1;
  }

public:
  vector<vector<string>> findLadders(const string &beginWord,
                                     const string &endWord,
                                     vector<string> &words) {
    int iBegin = -1, iEnd = -1, W = words.size();

    for (int i = 0; i < W; ++i)
      if (words[i] == endWord) {
        iEnd = i;
        break;
      }
    if (iEnd == -1)
      return {};

    for (int i = 0; i < W; ++i)
      if (words[i] == beginWord) {
        iBegin = i;
        break;
      }
    if (iBegin == -1) {
      iBegin = W++;
      words.push_back(beginWord);
    }

    // O(S W^2)
    vector<vector<int>> graph(W);
    for (int i = 0; i < W; ++i)
      for (int j = i + 1; j < W; ++j)
        if (i != j && neighbours(words[i], words[j]))
          graph[i].push_back(j), graph[j].push_back(i);

    vector<vector<int>> dijkstra(W);
    vector<bool> visited(W, false);
    visited[iBegin] = true;

    int dist = 0;
    vector<int> frontier, old_frontier;
    frontier.push_back(iBegin);

    while (!visited[iEnd]) {
      ++dist;
      swap(frontier, old_frontier);
      frontier.clear();

      // unreachable
      if (old_frontier.empty())
        return {};

      for (int f : old_frontier)
        for (int n : graph[f])
          if (!visited[n]) {
            if (dijkstra[n].empty())
              frontier.push_back(n);
            dijkstra[n].push_back(f);
          }

      for (int f : frontier)
        visited[f] = true;
    }

    using lambda_t = const function<vector<vector<string>>(int)>;
    lambda_t build_paths = [&](int n) -> vector<vector<string>> {
      if (n == iBegin)
        return {{beginWord}};

      vector<vector<string>> spaths;
      for (int prev : dijkstra[n]) {
        auto sspaths = build_paths(prev);
        for (vector<string> &sspath : sspaths)
          sspath.push_back(words[n]), spaths.push_back(move(sspath));
      };
      return spaths;
    };

    return build_paths(iEnd);
  }
};

// *****

void print_paths(const vector<vector<string>> &paths) {
  for (const vector<string> &path : paths) {
    for (const string &entry : path)
      cout << entry << ' ';
    cout << '\n';
  }
}

void test() {
  vector<tuple<string, string, vector<string>>> tests = {
      {
          "hit",
          "cog",
          {"hot", "dot", "dog", "lot", "log", "cog"},
      },
      {
          "hit",
          "cog",
          {"hot", "dot", "dog", "lot", "log"},
      },
      {
          "hot",
          "dog",
          {"hot", "dog"},
      },
  };

  const vector<vector<vector<string>>> expected = {
      {
          {"hit", "hot", "dot", "dog", "cog"},
          {"hit", "hot", "lot", "log", "cog"},
      },
      {},
      {},
  };

  bool dirty = false;
  for (size_t i = 0; i < tests.size(); ++i) {
    const auto paths = Solution{}.findLadders(
        get<0>(tests[i]), get<1>(tests[i]), get<2>(tests[i]));
    if (paths != expected[i]) {
      printf("Test #%lu failed\n", i);
      printf("> Expected: ... (%lu entries)\n", expected[i].size());
      print_paths(expected[i]);
      printf("> Actual:   ... (%lu entries)\n", paths.size());
      print_paths(paths);
      dirty = true;
    }
  }

  if (!dirty)
    cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
