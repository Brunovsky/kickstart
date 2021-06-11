#pragma once

#include "poly.hpp"

namespace polymath {

template <typename T>
auto faulhaber(T upto, int n) {
    vector<T> ex(n + 1);
    T e = 1;
    for (int i = 0; i <= n; i++) {
        ex[i] = e;
        e /= i + 1;
    }
    vector<T> den = ex;
    den.erase(den.begin());
    for (auto& d : den) {
        d = -d;
    }
    vector<T> num(n);
    T p = 1;
    for (int i = 0; i < n; i++) {
        p *= upto + 1;
        num[i] = ex[i + 1] * (1 - p);
    }
    vector<T> faul = truncated(num * inverse_series(den, n), n);
    T f = 1;
    for (int i = 0; i < n; i++) {
        faul[i] *= f;
        f *= i + 1;
    }
    return faul;
}

} // namespace polymath
