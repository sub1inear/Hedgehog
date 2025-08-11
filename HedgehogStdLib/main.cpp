#include "hstdlib.h"

int main() {
    auto [result, err] = println("Hello, world");
    print("%d", result);
}