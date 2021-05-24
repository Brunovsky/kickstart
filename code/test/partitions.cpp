#include "test_utils.hpp"
#include "../numeric/partitions.hpp"

inline namespace unit_testing_partitions {

void unit_test_choices() {
    vector<int> partition = first_choice(7, 5);
    do {
        print("{}\n", partition);
    } while (next_choice(partition, 7));
}

void unit_test_partitions() {
    vector<int> partition = first_unsized_partition(10);
    do {
        print("{}\n", partition);
    } while (next_unsized_partition(partition));
}

} // namespace unit_testing_partitions

int main() {
    RUN_BLOCK(unit_test_choices());
    RUN_BLOCK(unit_test_partitions());
    return 0;
}
