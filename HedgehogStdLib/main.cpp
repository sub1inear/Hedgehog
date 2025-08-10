#include "hstdlib.h"
#include <vector>

int main() {
    h_str str = "This is an interesting example to show how to use the find and replace mechanisms to do things.";
    println(str);
    str.replace_reverse("to", "two");
    print(str);
}