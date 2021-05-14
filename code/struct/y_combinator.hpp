#ifndef Y_COMBINATOR_HPP
#define Y_COMBINATOR_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Source: ecnerwala
 */
template <class Fun>
class y_combinator_result {
    Fun fun_;

  public:
    template <class T>
    explicit y_combinator_result(T&& fun) : fun_(std::forward<T>(fun)) {}

    template <class... Args>
    decltype(auto) operator()(Args&&... args) {
        return fun_(std::ref(*this), std::forward<Args>(args)...);
    }
};

template <class Fun>
auto y_combinator(Fun&& fun) {
    return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

#endif // Y_COMBINATOR_HPP
