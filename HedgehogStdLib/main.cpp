#include "hstdlib.h"

int main() {
    h_array<h_i32, 5> array = { 1, 2, 3, 4, 3 };

    print("%d", array.find(3));
    print("%d", array.find_reverse(3));
}