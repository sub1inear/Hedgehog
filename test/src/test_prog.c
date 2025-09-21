#include <stdio.h>

#include "unit.h"
#include "run.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <test_program>\n", argv[0]);
        return 1;
    }
    hhg_unit_assert(!hhg_run(argv[1]));
    return 0;
}
