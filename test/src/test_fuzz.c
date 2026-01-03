#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "unit.h"
#include "run.h"

int main(void)
{
    FILE *file = fopen("fuzz.hhg", "w");
    if (!file)
        hhg_fatal_error("fuzz.hhg: error opening file: %s", strerror(errno));

    srand(time(NULL));

    for (int32_t i = 0; i < 1000; i++)
        fputc((char)(rand() % 128), file);

    // put an invalid character to ensure failure
    // (just in case random data is valid)
    fputc('\1', file);

    fclose(file);
    
    // the fuzzed file is expected to cause a failure (or segfault)
    bool failed = hhg_run("fuzz.hhg");

    return failed ? EXIT_SUCCESS : EXIT_FAILURE;
}
