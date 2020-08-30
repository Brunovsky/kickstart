#!/usr/bin/env node

const fs = require("fs");
const assert = require("assert").strict;
const stdin = fs.readFileSync(0);

/**
 *             |  ^  &
 *  A  1, 1    1  0  1
 *  B  1, 0    1  1  0
 *  C  0, 1    1  1  0
 *  D  0, 0    0  0  0
 *
 * A,B->|  A,C->|  A,D->^  B,C->|  B,D->&  C,D->&
 * Therefore always 0 or 1. But still need to evaluate expression.
 */

const s = stdin.toString().trim().split("\n");
const T = +s[0];
assert(T == s.length - 1);

for (let t = 1; t <= T; t++) {
    s[t] = s[t].replace(/X/g, "(!x)");
    const x0 = eval(`const x=0; !!(${s[t]});`);
    const x1 = eval(`const x=1; !!(${s[t]});`);
    console.log(`Case #${t}: ${x0 == x1 ? "0" : "1"}`);
}
