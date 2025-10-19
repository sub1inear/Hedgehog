#include <stdlib.h>
#include <stdio.h>

#include "unit.h"
#include "run.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        char *prog_name = argv[0][0] == '\0' ? "test_prog" : argv[0];
        fprintf(stderr, "usage: %s <test_program>\n", prog_name);
        return 1;
    }
    hhg_unit_assert(!hhg_run(argv[1]));
    return EXIT_SUCCESS;
}
