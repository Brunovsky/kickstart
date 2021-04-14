#include "../numeric/bits.hpp"

#include "test_utils.hpp"

// *****

// clang-format off

void unit_test_bits() {
    assert(is_power_of_two(2));
    assert(is_power_of_two(32));
    assert(!is_power_of_two(3));
    assert(!is_power_of_two(7));

    assert(lsbits(7, 3) == "111");
    assert(lsbits(13, 4) == "1011");
    assert(lsbits(0b1001001, 7) == "1001001");
    assert(lsbits(0b101011010110, 12) == "011010110101");
    assert(lsbits(0, 7) == "0000000");
    assert(lsbits(32, 7) == "0000010");

    assert(msbits(13, 4) == "1101");
    assert(msbits(0b1001001, 7) == "1001001");
    assert(msbits(0b101011010110, 12) == "101011010110");
    assert(msbits(32, 7) == "0100000");

    uint v = 0b10000001000001000010001001011111;
    reverse_bits(v);
    assert(v == 0b11111010010001000010000010000001);
}

void unit_test_foreach_mask() {
    vector<uint> masks = {
        0b001111,
        0b010111,
        0b011011,
        0b011101,
        0b011110,
        0b100111,
        0b101011,
        0b101101,
        0b101110,
        0b110011,
        0b110101,
        0b110110,
        0b111001,
        0b111010,
        0b111100,
    };
    vector<uint> loop;
    FOR_EACH_MASK(mask, 4, 6)
        loop.push_back(mask);

    assert(loop == masks);
}

void unit_test_foreach_subset() {
    const uint mask = 0b0110110;
    vector<uint> masks = {
        0b0000010,
        0b0000100,
        0b0010000,
        0b0100000,
        0b0000110,
        0b0010010,
        0b0100010,
        0b0010100,
        0b0100100,
        0b0110000,
        0b0010110,
        0b0100110,
        0b0110010,
        0b0110100,
    };
    vector<uint> loop;
    FOR_EACH_SUBSET(subset, mask)
        loop.push_back(subset);

    sort(begin(loop), end(loop));
    sort(begin(masks), end(masks));
    assert(loop == masks);
}

int main() {
    RUN_SHORT(unit_test_bits());
    RUN_SHORT(unit_test_foreach_mask());
    RUN_SHORT(unit_test_foreach_subset());
    return 0;
}
