#include "test_utils.hpp"
#include "../struct/tensor.hpp"
#include "../linear/matrix.hpp"
#include "../lib/anynum.hpp"

inline namespace detail {

template <typename T>
auto multiply_tensors(const tensor<T, 2>& a, const tensor<T, 2>& b) {
    auto [N, M] = a.size();
    auto [Z, K] = b.size();
    assert(M == Z);
    tensor<T, 2> c({N, K}, 0);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < K; k++) {
                c[{i, k}] += a[{i, j}] * b[{j, k}];
            }
        }
    }
    return c;
}

template <typename T>
auto generate_mat(int N, int M, int v = 30) {
    mat<T> arr(N, M);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            arr[i][j] = uniform_gen<int>(-v, v);
        }
    }
    return arr;
}

template <typename T>
auto convert_to_tensor(const mat<T>& arr) {
    auto [N, M] = arr.size();
    tensor<T, 2> t({N, M});
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            t[{i, j}] = arr[i][j];
        }
    }
    return t;
}

} // namespace detail

inline namespace stress_testing_tensor {

void stress_test_tensor_multiply() {
    intd distN(150, 200);

    START_ACC(mat);
    START_ACC(tensor);

    LOOP_FOR_DURATION_OR_RUNS_TRACKED(5s, now, 100'000, runs) {
        print_time(now, 5s, 50ms, "stress test tensor");

        int N = distN(mt), M = distN(mt), K = distN(mt);
        mat<int> amat = generate_mat<int>(N, M, 1000);
        mat<int> bmat = generate_mat<int>(M, K, 1000);
        tensor<int, 2> aten = convert_to_tensor(amat);
        tensor<int, 2> bten = convert_to_tensor(bmat);

        START(mat);
        auto cmat = amat * bmat;
        ADD_TIME(mat);

        START(tensor);
        auto cten = multiply_tensors(aten, bten);
        ADD_TIME(tensor);

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < K; j++) {
                assert(cmat[i][j] == (cten[{i, j}]));
            }
        }
    }

    PRINT_TIME(mat);
    PRINT_TIME(tensor);
}

} // namespace stress_testing_tensor

int main() {
    RUN_BLOCK(stress_test_tensor_multiply());
    return 0;
}
