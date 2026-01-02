#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#define hhg_unit_assert(test) if (!(test)) abort();
#define hhg_unit_assert_msg(test, msg)                               \
            do {                                                     \
                if (!(test)) {                                       \
                    fprintf(                                         \
                        stderr,                                      \
                        "\x1b[1;31m" "hhg_unit_assert failed at "    \
                        "%s:%d: %s with %s" "\n" "\x1b[0m",          \
                        __FILE__,                                    \
                        __LINE__,                                    \
                        msg,                                         \
                        #test                                        \
                    );                                               \
                    abort();                                         \
                }                                                    \
            } while (0)
