#include "test_utils.hpp"
#include "../strings/strings.hpp"

inline namespace unit_testing_suffix_array {

void unit_test_suffix_array() {
    using vi = vector<int>;
    string s;
    vi sa, lcp;

    s = "abbaacaaabab"s;
    sa = build_cyclic_shifts(s, 256, '\0');
    lcp = build_lcp_array(s, sa);

    for (int i = 0, N = s.size(); i < N; i++) {
        print("{:2}: {}\n", i, s.substr(sa[i]) + s.substr(0, sa[i]));
    }
    print(" sa: {}\nlcp: {}\n", sa, lcp);
}

} // namespace unit_testing_suffix_array

int main() {
    RUN_SHORT(unit_test_suffix_array());
    return 0;
}
