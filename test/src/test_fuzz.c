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

    for (int32_t i = 0; i < 10000; i++)
        fputc((char)(rand() % 256), file);

    fputc('\1', file); // put an invalid character to ensure failure

    fclose(file);
    
    // the fuzzed file is expected to cause a failure (or segfault)
    bool result = hhg_run("fuzz.hhg");

    return result ? EXIT_FAILURE : EXIT_SUCCESS;
}
