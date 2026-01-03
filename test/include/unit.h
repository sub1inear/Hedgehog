#include <stdlib.h>
#include <stdio.h>

#include "msg.h"
#include "utils.h"
#define hhg_unit_assert(test) if (!(test)) abort()
#define hhg_unit_assert_msg(test, msg) if (!(test)) hhg_fatal_error(msg)
