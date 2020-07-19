// Useful debug code for the code.cpp file

void debug_assert() {
  assert(!tadj[T]);
  for (int trap = 0; trap < T; trap++) {
    assert(!(tadj[trap] & (1 << trap)));
    assert(!(~TMASK & tadj[trap]));
    assert(strength[trap] > 0);
    assert(trooms[trap].size() <= 4);
    for (int room : trooms[trap]) {
      if (room == room_S)
        assert(tstart & (1 << trap));
      if (room == room_T)
        assert(texit & (1 << trap));
      assert(rtraps[room].count(trap));
    }
  }
  for (int room = 0; room < N; room++) {
    assert(room_vis[room] == 0);
    assert(energy[room] >= 0);
    for (int trap : rtraps[room]) {
      assert(trooms[trap].count(room));
    }
  }
  for (int tmask = 0; tmask <= TMASK; tmask++) {
    for (int trap = 0; trap < T; trap++) {
      if (tmask & (1 << trap)) {
        assert(!(streach[tmask] & (1 << trap)));
      }
    }
  }
}

string sbits(uint n) {
  string s = string(T, '0');
  for (int b = 0; b < T; b++) {
    s[b] = (n & (1 << b)) ? '1' : '0';
  }
  return s;
}

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
