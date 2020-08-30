#!/usr/bin/env node

const fs = require("fs");
const stdin = fs.readFileSync(0);
const assert = require("assert").strict;

// single line template

const s = stdin.toString().trim().split("\n");
const T = +s[0];
assert(T == s.length - 1);
