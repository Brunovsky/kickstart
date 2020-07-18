#include <bits/stdc++.h>

using namespace std;

#define IS_TRAP(x) (-100000 < x && x < 0)
#define IS_OBSTACLE(x) (x <= -100000)
#define IS_ROOM(x) (0 <= x)

// *****

#define MAXR 102
#define MAXC 102
#define MAXN (MAXR * MAXC)
#define MAXT 16

constexpr int dd[2][4] = {{-1, 1, 0, 0}, {0, 0, -1, 1}};

int R, C, E;
int SR, SC, TR, TC;
int T; // num traps
int N; // num rooms

int reach_vis[MAXT];   // number of rooms reaching this trap
int trap_vis[MAXT];    // number of (recursive) visits
int strength[MAXT];    // strength of trap #i
set<int> tadj[MAXT];   // traps adjacent to trap #i
set<int> trooms[MAXT]; // rooms adjacent to trap #i

int room_vis[MAXN];    // number of (recursive) visits
int energy[MAXN];      // energy of room #i
set<int> rtraps[MAXN]; // traps adjacent to room #i

int cave[MAXR][MAXC];
int room_S; // room S is in
int room_T; // room T is in

int answer;

void print_cave() {
  for (int r = 0; r <= R + 1; r++) {
    for (int c = 0; c <= C + 1; c++) {
      if (r == SR && c == SC)
        putchar('S');
      else if (r == TR && c == TC)
        putchar('T');
      else if (IS_OBSTACLE(cave[r][c]))
        putchar('X');
      else if (IS_TRAP(cave[r][c]))
        putchar('o');
      else
        putchar(' ');
    }
    putchar('\n');
  }
}

void clean() {
  memset(reach_vis, 0, sizeof(reach_vis));
  memset(trap_vis, 0, sizeof(trap_vis));
  memset(strength, 0, sizeof(strength));
  for (int i = 0; i < MAXT; i++)
    tadj[i].clear();
  for (int i = 0; i < MAXT; i++)
    trooms[i].clear();
  memset(room_vis, 0, sizeof(room_vis));
  memset(energy, 0, sizeof(energy));
  for (int i = 0; i < MAXN; i++)
    rtraps[i].clear();
  memset(cave, 0xfe, sizeof(cave)); // obstacles everywhere
  answer = -1;
}

void setup() {
  int room = 1;
  int trap = 1;

  int ids[MAXR][MAXC] = {};
  bool vis[MAXR][MAXC] = {};

  auto floodfill_room = [&](int fr, int fc) {
    assert(IS_ROOM(cave[fr][fc]) && !vis[fr][fc]);
    ids[fr][fc] = room;

    list<array<int, 2>> frontier;
    frontier.push_back({fr, fc});

    while (!frontier.empty()) {
      auto cell = frontier.front();
      frontier.pop_front();
      int r = cell[0], c = cell[1];
      if (IS_ROOM(cave[r][c]) && !vis[r][c]) {
        frontier.push_back({r - 1, c});
        frontier.push_back({r + 1, c});
        frontier.push_back({r, c - 1});
        frontier.push_back({r, c + 1});
        energy[room] += cave[r][c]; // add the energy in the cell to the room
        if (r == SR && c == SC) {
          room_S = room;
        } else if (r == TR && c == TC) {
          room_T = room;
        }
        vis[r][c] = true;
        ids[r][c] = room;
      }
    }
    room++;
  };

  auto inspect_trap = [&](int r, int c) {
    assert(IS_TRAP(cave[r][c]) && !vis[r][c]);
    ids[r][c] = trap;
    strength[trap] = -cave[r][c];

    for (int i = 0; i < 4; i++) {
      int dr = r + dd[0][i], dc = c + dd[1][i];
      if (IS_ROOM(cave[dr][dc])) {
        if (!vis[dr][dc]) {
          floodfill_room(dr, dc);
        }
        int id = ids[dr][dc];
        trooms[trap].insert(id);
        rtraps[id].insert(trap);
      }
      // fancy, two traps adjacent to each other. ignore if we're first.
      if (IS_TRAP(cave[dr][dc]) && vis[dr][dc]) {
        int id = ids[dr][dc];
        tadj[trap].insert(id);
        tadj[id].insert(trap);
      }
    }
    vis[r][c] = true;
    trap++;
  };

  for (int r = 1; r <= R; r++) {
    for (int c = 1; c <= C; c++) {
      if (vis[r][c] || IS_OBSTACLE(cave[r][c])) {
        continue;
      }
      if (IS_TRAP(cave[r][c])) {
        inspect_trap(r, c);
      } else if (IS_ROOM(cave[r][c])) {
        floodfill_room(r, c);
      }
      assert(vis[r][c]);
    }
  }

  T = trap - 1;
  N = room - 1;
  assert(T < MAXT && N < MAXN && N > 0);
}

void build_graph() {
  // phantom trap at S with strength 0
  trooms[0].insert(room_S);
  for (int trap : rtraps[room_S]) {
    tadj[0].insert(trap);
  }
  // connect traps
  for (int room = 1; room <= N; room++) {
    for (int trap1 : rtraps[room]) {
      for (int trap2 : rtraps[room]) {
        if (trap1 != trap2) {
          tadj[trap1].insert(trap2);
          tadj[trap2].insert(trap1);
        }
      }
    }
  }
}

bool visit_room(int room) {
  if (!room_vis[room]++) {
    E += energy[room];
    if (room_vis[room_T]) {
      answer = max(answer, E);
    }
  }
  return room_vis[room] == 1;
}

void unvisit_room(int room) {
  assert(room_vis[room] > 0);
  if (!--room_vis[room]) {
    E -= energy[room];
  }
}

void visit_trap(int trap) {
  assert(!trap_vis[trap]);

  // not enough energy
  if (strength[trap] > E) {
    return;
  }

  E -= strength[trap];
  int new_rooms = 0;

  // visit new rooms
  for (int room : trooms[trap]) {
    new_rooms += visit_room(room);
  }

  if (new_rooms > 0) {
    for (int adj_trap : tadj[trap]) {
      reach_vis[adj_trap]++;
    }
    trap_vis[trap]++;
    // visit any reachable trap unvisited
    for (int any_trap = 1; any_trap <= T; any_trap++) {
      if (!trap_vis[any_trap] && reach_vis[any_trap]) {
        visit_trap(any_trap);
      }
    }
    trap_vis[trap]--;
    for (int adj_trap : tadj[trap]) {
      reach_vis[adj_trap]--;
    }
  }

  // unvisit new rooms
  for (int room : trooms[trap]) {
    unvisit_room(room);
  }

  E += strength[trap];
}

auto solve() {
  cin >> R >> C >> E >> SR >> SC >> TR >> TC;
  clean();
  for (int r = 1; r <= R; r++) {
    for (int c = 1; c <= C; c++) {
      cin >> cave[r][c] >> ws;
    }
  }
  setup();
  build_graph();
  visit_trap(0);
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
