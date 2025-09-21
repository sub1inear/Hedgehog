#include <stdio.h>
#include <string.h>

#include "minunit.h"
#include "run.h"


static const char *test_prog(const char *filename)
{
    mu_assert(filename, hhg_run(filename));
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <test_program>\n", argv[0]);
        return 1;
    }
    return test_prog(argv[1]) != NULL;
}