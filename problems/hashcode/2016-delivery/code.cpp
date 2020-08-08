#include <bits/stdc++.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;
using namespace fmt;

// *****

#define MAXD 1000
#define MAXP 2000
#define MAXW 20
#define MAXH 2000
ifstream fin;
ofstream fout;

struct int2 {
    int r, c;
};

enum cmd_t : char { LOAD = 'L', UNLOAD = 'U', DELIVER = 'D', WAIT = 'W', MOVE = 'M' };
enum place_t : int8_t { DEPOT = 0, HOME = 1 };

using drone_queue_t = priority_queue<int, vector<int>, function<bool(int, int)>>;

ostream& operator<<(ostream& out, int2 pos) {
    return out << format("({},{})", pos.r, pos.c);
}

inline int dist(int2 a, int2 b) {
    int dr = a.r - b.r, dc = a.c - b.c;
    return ceil(sqrt(dr * dr + dc * dc));
}

struct Warehouse {
    int id;
    int2 cell;
    int supply[MAXP];
    int total;
};

struct Home {
    int id;
    int2 cell;
    int demand[MAXP];
    int total;
    int weight;
    int time;
    vector<int> items;
};

struct Move {
    int drone;
    cmd_t type;
    int destid, item = 0, amount = 0;
};

struct Drone {
    int id;
    int2 cell;
    int time;
    int load;
    int carry[MAXP];
    vector<Move> moves;
};

int weightof[MAXP];

bool less_weights(int item1, int item2) {
    int w1 = weightof[item1], w2 = weightof[item2];
    return w1 < w2 || (w1 == w2 && item1 < item2);
}

// *****

struct Solver {
    int R, C;
    int D;                  // drones
    int T;                  // time
    int L;                  // maximum payload
    int P;                  // item kinds
    int W;                  // depots
    int H;                  // homes or orders
    Warehouse depots[MAXW]; // depot (warehouse) data
    Home homes[MAXH];       // order (home) data
    Drone drones[MAXD];     // drone data
    drone_queue_t drone_queue;
    int total_moves = 0;

    void statistics() {
        print("Size: {}x{}\n", R, C);
        print("Drones: {}\n", D);
        print("Time: {}\n", T);
        print("Maximum payload: {}\n", L);
        print("Product kinds: {}\n", P);
        print("Warehouses: {}\n", W);
        print("Orders: {}\n", H);
        print("\n");

        // Supply and demand
        int total_supply = 0;
        int total_demand = 0;
        int total_weight = 0;
        int total_trips = 0;
        int min_supply = INT_MAX, max_supply = 0;
        int min_demand = INT_MAX, max_demand = 0;
        int min_weight = INT_MAX, max_weight = 0;
        for (int w = 0; w < W; w++) {
            min_supply = min(min_supply, depots[w].total);
            max_supply = max(max_supply, depots[w].total);
            total_supply += depots[w].total;
        }
        for (int h = 0; h < H; h++) {
            min_demand = min(min_demand, homes[h].total);
            max_demand = max(max_demand, homes[h].total);
            total_demand += homes[h].total;
            min_weight = min(min_weight, homes[h].weight);
            max_weight = max(max_weight, homes[h].weight);
            total_weight += homes[h].weight;
            total_trips += (homes[h].weight + L - 1) / L;
        }
        print("Total supply: {}\n", total_supply);
        print("Avg supply/warehouse: {:.1f}\n", 1.0 * total_supply / W);
        print("Supply range: [{},{}]\n\n", min_supply, max_supply);

        print("Total demand: {}\n", total_demand);
        print("Avg demand/home: {:.1f}\n", 1.0 * total_demand / H);
        print("Demand range: [{},{}]\n\n", min_demand, max_demand);

        print("Total weight: {}\n", total_weight);
        print("Avg weight/home: {:.1f}\n", 1.0 * total_weight / H);
        print("Order weight range: [{},{}]\n\n", min_weight, max_weight);

        print("Minimum trips: {}\n", total_trips);
        print("Avg min trips/home: {:.1f}\n", 1.0 * total_trips / H);
        print("Avg min trips/drone: {:.1f}\n", 1.0 * total_trips / D);
        print("\n");

        // Product weights
        int min_product_weight = *min_element(weightof, weightof + P);
        int max_product_weight = *max_element(weightof, weightof + P);
        double avg_product_weight = 1.0 * accumulate(weightof, weightof + P, 0) / P;
        print("Weight range: [{},{}]\n", min_product_weight, max_product_weight);
        print("Avg product weight: {}\n", avg_product_weight);
        print("\n");

        // Order distances
        double dist_close = 0;
        double dist_every = 0;
        for (int h = 0; h < H; h++) {
            int min_dist = INT_MAX;
            for (int w = 0; w < W; w++) {
                min_dist = min(min_dist, dist(homes[h].cell, depots[w].cell));
                dist_every += dist(homes[h].cell, depots[w].cell);
            }
            dist_close += min_dist;
        }
        dist_close /= H, dist_every /= H;
        print("Avg distance to close warehouse: {:.1f}\n", dist_close);
        print("Avg distance to every warehouse: {:.1f}\n", dist_every);
        print("\n");

        // Geographic distribution
        int2 sz = {80 * R / C, 80};
        vector<string> mat(sz.r, string(sz.c, ' '));

        // Warehouses
        for (int w = 0; w < W; w++) {
            const auto& depot = depots[w];
            char c = 'A' + w;
            mat[depot.cell.r * sz.r / R][depot.cell.c * sz.c / C] = c;
            print("{} ({:03},{:03}) - {}\n", c, depot.cell.r, depot.cell.c, depot.total);
        }
        print("Warehouse distribution\n");
        print("+{}+\n", string(sz.c, '-'));
        for (int r = 0; r < sz.r; r++)
            print("|{}|\n", mat[r]);
        print("+{}+\n", string(sz.c, '-'));

        // Orders
        vector<vector<int>> cnt(sz.r, vector<int>(sz.c, 0));
        for (int h = 0; h < H; h++) {
            const auto& home = homes[h];
            cnt[home.cell.r * sz.r / R][home.cell.c * sz.c / C] += home.weight;
        }
        int concentration = 0;
        for (auto& row : cnt) {
            concentration = max(concentration, *max_element(begin(row), end(row)));
        }
        print("Max concentration of demand weight: {}\n", concentration);
        int div = (concentration + 9) / 10;
        for (int r = 0; r < sz.r; r++) {
            for (int c = 0; c < sz.c; c++) {
                if (cnt[r][c]) {
                    mat[r][c] = '0' + (cnt[r][c] - 1) / div;
                } else {
                    mat[r][c] = ' ';
                }
            }
        }
        print("Orders distribution\n");
        print("+{}+\n", string(sz.c, '-'));
        for (int r = 0; r < sz.r; r++)
            print("|{}|\n", mat[r]);
        print("+{}+\n", string(sz.c, '-'));
    }

    void drone_statistics() {
        for (int i = 0; i < D; i++) {
            print("{:2} == time: {:<6}   moves: {}\n", i, drones[i].time,
                  drones[i].moves.size());
        }
    }

    void read() {
        fin >> R >> C >> D >> T >> L;
        fin >> P;
        for (int i = 0; i < P; i++) {
            fin >> weightof[i];
        }
        fin >> W;
        for (int w = 0; w < W; w++) {
            auto& depot = depots[w];
            depot.id = w;
            fin >> depot.cell.r >> depot.cell.c;
            for (int i = 0; i < P; i++) {
                fin >> depot.supply[i];
            }
            depot.total = accumulate(depot.supply, depot.supply + P, 0);
        }
        fin >> H;
        for (int h = 0; h < H; h++) {
            auto& home = homes[h];
            fin >> home.cell.r >> home.cell.c >> home.total;
            home.items.resize(home.total);
            for (int j = 0; j < home.total; j++) {
                int item;
                fin >> item;
                home.items[j] = item;
                home.demand[item]++;
                home.weight += weightof[item];
            }
            sort(begin(home.items), end(home.items), less_weights);
        }
    }

    void setup() {
        for (int h = 0; h < H; h++) {
            homes[h].time = T;
        }
        for (int i = 0; i < D; i++) {
            drones[i].id = i;
            drones[i].cell = depots[0].cell;
            drones[i].time = drones[i].load = 0;
            memset(drones[i].carry, 0, sizeof(drones[i].carry));
        }
        // max heap
        drone_queue = drone_queue_t([this](int i1, int i2) {
            int t1 = drones[i1].time, t2 = drones[i2].time;
            return t1 > t2 || (t1 == t2 && i1 > i2);
        });
        for (int i = 0; i < D; i++) {
            drone_queue.push(i);
        }
    }

    int queue_pop() {
        int i = drone_queue.top();
        drone_queue.pop();
        return i;
    }

    void queue_push(int i) {
        if (drones[i].time < T) {
            drone_queue.push(i);
        }
    }

    bool queue_empty() {
        return drone_queue.empty();
    }

    void load(int i, int w, int item, int amount) {
        assert(i < D && w < W && item < P && amount > 0);
        assert(amount <= depots[w].supply[item]);
        assert(drones[i].load + weightof[item] * amount <= L);

        drones[i].moves.push_back({i, LOAD, w, item, amount});
        depots[w].supply[item] -= amount;
        drones[i].carry[item] += amount;
        drones[i].load += weightof[item] * amount;

        drones[i].time += 1 + dist(drones[i].cell, depots[w].cell);
        drones[i].cell = depots[w].cell;
        total_moves++;
    }

    void unload(int i, int w, int item, int amount) {
        assert(i < D && w < W && item < P && amount > 0);
        assert(amount <= drones[i].carry[item]);

        drones[i].moves.push_back({i, UNLOAD, w, item, amount});
        depots[w].supply[item] += amount;
        drones[i].carry[item] -= amount;
        drones[i].load -= weightof[item] * amount;
        depots[w].total += amount;

        drones[i].time += 1 + dist(drones[i].cell, depots[w].cell);
        drones[i].cell = depots[w].cell;
        total_moves++;
    }

    void deliver(int i, int h, int item, int amount) {
        assert(i < D && h < H && item < P && amount > 0);
        assert(amount <= drones[i].carry[item]);
        assert(amount <= homes[h].demand[item]);

        drones[i].moves.push_back({i, DELIVER, h, item, amount});
        homes[h].demand[item] -= amount;
        drones[i].carry[item] -= amount;
        drones[i].load -= weightof[item] * amount;
        homes[h].total -= amount;

        drones[i].time += 1 + dist(drones[i].cell, homes[h].cell);
        drones[i].cell = homes[h].cell;
        total_moves++;

        if (homes[h].total == 0) {
            homes[h].time = min(T, drones[i].time);
        }
    }

    void wait(int i, int duration) {
        assert(i < D && duration > 0);
        drones[i].moves.push_back({i, WAIT, duration});
        drones[i].time += duration;
    }

    void move(int i, int w) {
        assert(i < D && w < W);
        drones[i].moves.push_back({i, MOVE, w});
        drones[i].time += dist(drones[i].cell, depots[w].cell);
        drones[i].cell = depots[w].cell;
    }

    int compute_score() {
        int score = 0;
        for (int h = 0; h < H; h++) {
            if (homes[h].time <= T) {
                score += ceil(100.0 * (T - homes[h].time) / T);
            }
        }
        return score;
    }

    void write() {
        fout << format("{}\n", total_moves);
        for (int i = 0; i < D; i++) {
            const auto& moves = drones[i].moves;
            for (uint m = 0; m < moves.size(); m++) {
                auto [id, type, destid, item, amount] = moves[m];
                assert(i == id);
                if (type == LOAD || type == UNLOAD || type == DELIVER) {
                    fout << format("{} {} {} {} {}\n", i, type, destid, item, amount);
                } else if (type == WAIT) {
                    fout << format("{} W {}\n", i, destid);
                }
            }
        }
    }
};

// *****

struct OneWarehouse : Solver {
    int closest[MAXH];

    void solve() {
        iota(closest, closest + H, 0);
        sort(closest, closest + H, [&](int id1, int id2) {
            int d1 = dist(homes[id1].cell, depots[0].cell);
            int d2 = dist(homes[id2].cell, depots[0].cell);
            return d1 < d2 || (d1 == d2 && id1 < id2);
        });

        int o = 0;

        while (!queue_empty() && o < H) {
            int i = queue_pop();
            fill_drone(i, closest[o]);

            while (o < H && homes[closest[o]].total == 0) {
                o++;
            }
            queue_push(i);
        }
    }

    void fill_drone(int i, int h) {
        assert(i < D && h < H && drones[i].load == 0);
        unordered_map<int, int> items;
        vector<int> indices;
        int drone_load = 0;
        for (auto it = homes[h].items.begin(); it != homes[h].items.end();) {
            int item = *it;
            if (weightof[item] + drone_load <= L) {
                it = homes[h].items.erase(it);
                items[item]++;
                drone_load += weightof[item];
            } else {
                ++it;
            }
        }
        // load all items, deliver them and return
        for (auto [item, amount] : items) {
            load(i, 0, item, amount);
        }
        for (auto [item, amount] : items) {
            deliver(i, h, item, amount);
        }
        move(i, 0);
    }
};

// *****

OneWarehouse solver;

void solve() {
    solver.read();
    solver.setup();
    solver.statistics();
    solver.solve();
    print("Score: {}\n", solver.compute_score());
    solver.write();
    solver.drone_statistics();
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        print("Missing input filename\n");
        return 1;
    }
    string filename = argv[1];
    assert(regex_match(filename, regex("^input/.*$")));

    fin.open(filename);
    if (!fin.is_open()) {
        print("Failed to open input '{}'\n", filename);
        return 1;
    }
    print("Input: {}\n", filename);

    filename = regex_replace(filename, regex("^input/"), "output/");
    fout.open(filename);
    if (!fout.is_open()) {
        print("Failed to open output '{}'\n", filename);
        return 1;
    }
    print("Output: {}\n", filename);

    solve();
    return 0;
}
