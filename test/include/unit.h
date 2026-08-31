#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include "utils.h"

#define hhg_unit_assert(test)                                                  \
    do                                                                         \
        if (!(test))                                                           \
            abort();                                                           \
    while (0)
#define hhg_unit_assert_msg(test, msg)                                         \
    do                                                                         \
        if (!(test))                                                           \
            hhg_fatal_error(msg);                                              \
    while (0)
