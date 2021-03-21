#ifndef BIGINT_EXTRA_HPP
#define BIGINT_EXTRA_HPP

#include "bigint.hpp"

// *****

bigint& operator&=(bigint& u, const bigint& v) {
    int n = min(u.len(), v.len());
    u.nums.resize(n);
    for (int i = 0; i < n; i++)
        u[i] = u[i] & v[i];
    u.trim();
    return u;
}
bigint& operator|=(bigint& u, const bigint& v) {
    int n = max(u.len(), v.len());
    u.nums.resize(n, 0);
    for (int i = 0; i < v.len(); i++)
        u[i] = u[i] | v[i];
    return u;
}
bigint& operator^=(bigint& u, const bigint& v) {
    int n = max(u.len(), v.len());
    u.nums.resize(n, 0);
    for (int i = 0; i < v.len(); i++)
        u[i] = u[i] ^ v[i];
    u.trim();
    return u;
}
bigint operator~(bigint u) {
    for (int i = 0; i < u.len(); i++)
        u[i] = ~u[i];
    u.trim();
    return u;
}

bigint operator&(bigint u, const bigint& v) { return u &= v; }
bigint operator|(bigint u, const bigint& v) { return u |= v; }
bigint operator^(bigint u, const bigint& v) { return u ^= v; }

#endif // BIGINT_EXTRA_HPP
