#include "../algo/strings.hpp"
#include "test_utils.hpp"

void unit_test_suffix_array() {
    using vi = vector<int>;
    string s;
    vi sa, lcp;

    s = "abbaacaaabab"s;
    sa = build_cyclic_shifts(s, 256, '\0');
    lcp = build_lcp_array(s, sa);
    for (int i = 0, N = s.size(); i < N; i++)
        cout << setw(2) << i << ": " << s.substr(sa[i]) + s.substr(0, sa[i]) << endl;
    cout << sa << endl << lcp << endl;
}

int main() {
    RUN_SHORT(unit_test_suffix_array());
    return 0;
}
