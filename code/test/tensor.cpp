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

void speed_test_tensor_multiply() {
    intd distN(150, 200);

    START_ACC2(mat, tensor);

    LOOP_FOR_DURATION_OR_RUNS_TRACKED (5s, now, 100'000, runs) {
        print_time(now, 5s, "stress test tensor x vvi");

        int N = distN(mt), M = distN(mt), K = distN(mt);
        mat<unsigned> amat = generate_mat<unsigned>(N, M, 1000000);
        mat<unsigned> bmat = generate_mat<unsigned>(M, K, 1000000);
        tensor<unsigned, 2> aten = convert_to_tensor(amat);
        tensor<unsigned, 2> bten = convert_to_tensor(bmat);

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

    PRINT_EACH(mat, runs);
    PRINT_EACH(tensor, runs);
}

int main() {
    RUN_BLOCK(speed_test_tensor_multiply());
    return 0;
}
