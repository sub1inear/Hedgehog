#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "run.h"
#include "error.h"

int main(void)
{
    return hhg_run("../test/testcases/format.hhg");
}
