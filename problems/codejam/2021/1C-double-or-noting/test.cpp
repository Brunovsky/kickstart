#include <iostream>
using namespace std;

#define flip(x) (x ? ~x & ((1 << (31 - __builtin_clz(x))) - 1) : 0)
int main() {
    cout << flip(0b00) << endl;
    cout << flip(0b01) << endl;
    cout << flip(0b10) << endl;
    cout << flip(0b110) << endl;
    cout << flip(0b101) << endl;
    cout << flip(0b111) << endl;
    cout << flip(0b1011) << endl;
    return 0;
}
