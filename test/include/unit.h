#include <stdlib.h>
#include <stdio.h>
#define hhg_unit_assert(test) if (!(test)) return EXIT_FAILURE;
#define hhg_unit_assert_msg(test, msg) \
            do { if (!(test)) { \
                    fputs("\x1b[1;31m" "hhg_unit_assert failed: " \
                          msg "\n" "\x1b[0m", stderr); \
                    return EXIT_FAILURE; \
                } \
            } while (0)
