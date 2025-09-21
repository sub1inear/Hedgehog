#include <stdio.h>
#define hhg_unit_assert(test) if (!(test)) return 1;
#define hhg_unit_assert_msg(test, msg) do { if (!(test)) { \
                                                fputs(msg, stderr); \
                                                return 1; \
                                            } \
                                       } while (0)