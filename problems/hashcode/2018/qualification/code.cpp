#include "core.hpp"
#include "io/print.hpp"

#include <bits/stdc++.h>

using namespace std;
using namespace hack;

// *****

constexpr u16 RMAX = 10'000;
constexpr u16 CMAX = 10'000;
constexpr u32 MAX_NUM_CABS = 1'000;
constexpr u32 MAX_NUM_RIDES = 10'000;
constexpr u32 MAX_BONUS = 10'000;
constexpr u32 MAX_SIM_STEPS = 1'000'000'000;

u32 grid[RMAX][CMAX];

u16 R, C;
u32 num_cabs;   // fleet size
u32 num_rides;  // number of rides
u32 bonus;      // per-ride bonus for starting the ride on time
u32 sim_steps;  // number of steps in the simulation

struct Ride {
  u16 r0, c0;
  u16 r1, c1;
  u32 start, finish;
  u16 id;
};

Ride rides[MAX_NUM_RIDES];

// *****

void solve() {}

// *****

void parse_input() {
  cin >> R >> C >> num_cabs >> num_rides >> bonus >> sim_steps >> ws;
  for (u32 i = 0; i < num_rides; ++i) {
    Ride ride;
    ride.id = i;
    cin >> ride.r0 >> ride.c0 >> ride.r1 >> ride.c1;
    cin >> ride.start >> ride.finish >> ws;
    rides[i] = ride;
  }
}

// *****

int main() {
  parse_input();
  solve();
  return 0;
}
