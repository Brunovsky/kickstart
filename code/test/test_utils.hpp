#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "lib/test_chrono.hpp"
#include "lib/test_progress.hpp"
#include "../random.hpp"
#include "../linear/matrix.hpp"

template <typename Container>
bool all_eq(const Container& v) {
    return v.empty() || equal(next(begin(v)), end(v), begin(v));
}

template <typename U, typename V>
mat<string> make_table(const vector<U>& u, const vector<V>& v, string s = "") {
    int N = u.size(), M = v.size();
    mat<string> mat(N + 1, M + 1, s);
    for (int i = 0; i < N; i++) {
        if constexpr (is_same_v<U, string>) {
            mat[i + 1][0] = u[i];
        } else {
            mat[i + 1][0] = to_string(u[i]);
        }
    }
    for (int j = 0; j < M; j++) {
        if constexpr (is_same_v<V, string>) {
            mat[0][j + 1] = v[j];
        } else {
            mat[0][j + 1] = to_string(v[j]);
        }
    }
    return mat;
}

#define RUN_BLOCK(test)                                        \
    do {                                                       \
        print("{:<10} === {}\n", "RUN", #test);                \
        START(runner);                                         \
        test;                                                  \
        TIME(runner);                                          \
        clear_line();                                          \
        print("OK {:>6.2f}s === {}\n", TIME_S(runner), #test); \
    } while (0)

#define RUN_SHORT(test)                                        \
    do {                                                       \
        START(runner);                                         \
        test;                                                  \
        TIME(runner);                                          \
        clear_line();                                          \
        print("OK {:>6.2f}s === {}\n", TIME_S(runner), #test); \
    } while (0)

#endif // TEST_UTILS_HPP
