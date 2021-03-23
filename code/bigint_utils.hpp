#ifndef BIGINT_UTILS_HPP
#define BIGINT_UTILS_HPP

#include "bigint.hpp"

// *****

string lsbits(const bigint& u) {
    if (u.zero())
        return "0";
    string s(32 * u.len() + 1, '0');
    s[0] = u.sign ? '-' : '+';
    for (int i = 0; i < 32 * u.len(); i++)
        s[i + 1] = '0' + u.bit(i);
    while (!s.empty() && s.back() == '0')
        s.pop_back();
    return s;
}

string msbits(const bigint& u) {
    if (u.zero())
        return "0";
    string s(32 * u.len() + 1, '0');
    s[0] = u.sign ? '-' : '+';
    for (int i = 0; i < 32 * u.len(); i++)
        s[32 * u.len() - i] = '0' + u.bit(i);
    s.erase(begin(s) + 1, find(begin(s) + 1, end(s), '1'));
    return s;
}

string to_string(bigint u, uint b = 10) {
    static auto uint_to_string = [](uint n, uint base) {
        string s;
        while (n > 0) {
            uint m = n / base;
            s += '0' + (n - base * m), n = m;
        }
        reverse(begin(s), end(s));
        return s;
    };

    if (u.zero())
        return "0";
    string s = u.sign ? "-" : "";
    vector<string> rems;
    uint divisor = b, digits = 1;
    while (divisor < UINT_MAX / b) {
        divisor *= b, digits++;
    }
    while (!u.zero()) {
        rems.push_back(uint_to_string(div_int(u, divisor), b));
    }
    for (int i = 0, n = rems.size(); i < n; i++) {
        string pad(i ? digits - rems[n - i - 1].length() : 0, '0');
        s += pad + rems[n - i - 1];
    }
    return s;
}

ostream& operator<<(ostream& out, const bigint& u) { return out << to_string(u); }

#endif // BIGINT_UTILS_HPP
