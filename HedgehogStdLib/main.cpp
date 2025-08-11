#include "hstdlib.h"

int main() {
    h_random r = h_random();
    for (h_i32 i = 0; i < 100; i++) {
        println("%u", r.random(10, 20));
    }

}