#include "testlib.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int genValue(int low, int high, const string& mode) {
    ensuref(low <= high, "invalid genValue range [%d, %d]", low, high);
    if (low == high) return low;
    if (mode == "minimum") return low;
    if (mode == "maximum") return high;
    if (mode == "boundary") return rnd.next(0, 1) ? low : high;
    return rnd.next(low, high);
}

int main(int argc, char* argv[]) {
    registerGen(argc, argv, 1);
    prepareOpts(argc, argv);

    const int subtask = opt<int>("subtask");
    const double rate = opt<double>("rate");
    const string profile = opt<string>("profile");

    ensuref(subtask == 1 || subtask == 2, "subtask must be 1 or 2");
    ensuref(0.70 <= rate && rate <= 1.00, "rate must be in [0.70, 1.00]");

    const vector<string> profiles = {
        "minimum", "open", "near-open", "random", "dense",
        "checkerboard", "long-only", "zero-answer", "boundary"
    };
    ensuref(find(profiles.begin(), profiles.end(), profile) != profiles.end(),
            "unknown profile: %s", profile.c_str());

    const int maxR = (subtask == 1 ? 8 : 2087);
    const int maxC = (subtask == 1 ? 7 : 2087);
    const int scaledR = max(1, static_cast<int>(llround(maxR * rate)));
    const int scaledC = max(1, static_cast<int>(llround(maxC * rate)));

    int r = 1, c = 1;
    vector<string> grid;

    auto chooseLargeDimension = [](int scaled) {
        const int low = max(1, static_cast<int>(ceil(scaled * 0.85)));
        return genValue(low, scaled, rnd.next(0, 3) == 0 ? "maximum" : "uniform");
    };
    auto makeGrid = [&](char fill) {
        grid.assign(r, string(c, fill));
    };
    auto reopenEndpoints = [&]() {
        grid[0][0] = '.';
        grid[r - 1][c - 1] = '.';
    };
    auto addRandomObstacles = [&](int percent) {
        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < c; ++j) {
                grid[i][j] = (rnd.next(0, 99) < percent ? '#' : '.');
            }
        }
        reopenEndpoints();
    };

    if (profile == "minimum") {
        const int capR = min(4, scaledR);
        const int capC = min(4, scaledC);
        const int shape = rnd.next(0, 4);
        if (shape == 0) {
            r = c = 1;
        } else if (shape == 1) {
            r = 1;
            c = genValue(1, capC, "boundary");
        } else if (shape == 2) {
            r = genValue(1, capR, "boundary");
            c = 1;
        } else {
            r = genValue(1, capR, "uniform");
            c = genValue(1, capC, "uniform");
        }
        makeGrid('.');
        if (r * c > 1 && rnd.next(0, 1)) addRandomObstacles(rnd.next(20, 75));
        reopenEndpoints();
    } else if (profile == "long-only") {
        if (rnd.next(0, 1) == 0) {
            r = 2;
            c = 4;
        } else {
            r = 4;
            c = 2;
        }
        makeGrid('#');
        reopenEndpoints();
    } else if (profile == "zero-answer") {
        r = genValue(4, scaledR, "maximum");
        c = genValue(4, scaledC, "maximum");
        makeGrid('.');
        addRandomObstacles(rnd.next(10, 50));
        const int dr[4] = {0, 1, 1, 3};
        const int dc[4] = {1, 0, 3, 1};
        for (int k = 0; k < 4; ++k) {
            const int x = dr[k], y = dc[k];
            if (x < r && y < c) grid[x][y] = '#';
        }
        reopenEndpoints();
    } else {
        if (profile == "boundary") {
            r = scaledR;
            c = scaledC;
        } else {
            r = chooseLargeDimension(scaledR);
            c = chooseLargeDimension(scaledC);
            if (profile != "open" && rnd.next(0, 9) == 0) {
                if (rnd.next(0, 1)) r = rnd.next(1, min(4, scaledR));
                else c = rnd.next(1, min(4, scaledC));
            }
        }

        makeGrid('.');
        if (profile == "open") {
            // Repeated '.' rows maximize branching.
        } else if (profile == "near-open") {
            addRandomObstacles(rnd.next(2, 10));
        } else if (profile == "random") {
            addRandomObstacles(rnd.next(15, 55));
        } else if (profile == "dense") {
            addRandomObstacles(rnd.next(70, 92));
        } else if (profile == "checkerboard") {
            const int period = rnd.next(2, 5);
            const int phase = rnd.next(0, period - 1);
            for (int i = 0; i < r; ++i) {
                for (int j = 0; j < c; ++j) {
                    grid[i][j] = ((i + j + phase) % period == 0 ? '#' : '.');
                }
            }
            if (r * c > 2) {
                const int x = rnd.next(0, r - 1);
                const int y = rnd.next(0, c - 1);
                grid[x][y] = (grid[x][y] == '#' ? '.' : '#');
            }
        } else {  // boundary
            const int pattern = rnd.next(0, 3);
            if (pattern == 1) {
                const int run = rnd.next(2, 9);
                for (int i = 0; i < r; ++i)
                    for (int j = 0; j < c; ++j)
                        if (((j / run) + i) % 5 == 0) grid[i][j] = '#';
            } else if (pattern == 2) {
                for (int i = 0; i < r; ++i)
                    for (int j = 0; j < c; ++j)
                        if ((i + j) % 2 == 0) grid[i][j] = '#';
            } else if (pattern == 3) {
                addRandomObstacles(rnd.next(20, 60));
            }
        }
        reopenEndpoints();
    }

    ensuref(1 <= r && r <= maxR && 1 <= c && c <= maxC,
            "generated dimensions violate the selected subtask");
    ensuref(static_cast<int>(grid.size()) == r, "wrong number of rows");
    for (const string& row : grid) {
        ensuref(static_cast<int>(row.size()) == c, "wrong row length");
        for (char ch : row) ensuref(ch == '.' || ch == '#', "invalid grid character");
    }
    ensuref(grid[0][0] == '.' && grid[r - 1][c - 1] == '.',
            "start and target must be open");
    if (profile == "long-only") {
        ensuref((r == 2 && c == 4) || (r == 4 && c == 2),
                "long-only has an invalid shape");
        int openCells = 0;
        for (const string& row : grid)
            openCells += count(row.begin(), row.end(), '.');
        ensuref(openCells == 2, "long-only must have exactly two open cells");
    }
    if (profile == "zero-answer") {
        const int dr[4] = {0, 1, 1, 3};
        const int dc[4] = {1, 0, 3, 1};
        for (int k = 0; k < 4; ++k)
            ensuref(grid[dr[k]][dc[k]] == '#', "zero-answer successor is open");
    }

    cout << r << ' ' << c << '\n';
    for (const string& row : grid) cout << row << '\n';
    return 0;
}
