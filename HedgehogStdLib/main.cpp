#include "hstdlib.h"

int main() {
    h_i32 i;
    auto [result, err] = scan("%d", &i);
}