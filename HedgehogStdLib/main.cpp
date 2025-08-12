#include "hstdlib.h"

int main() {
    h_file file = h_file("hstdlib.h", "r");
    h_array<h_char, 3> x;
    file.read(x, 2);
    print(x);
}